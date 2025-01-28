#include "HX711.h"

// Pines del HX711
#define DT 16
#define SCK 10

HX711 balanza;

// Configuraciones iniciales
float factorCalibracion = -15.32; // Factor de calibración
String unidad = "kg";             // Unidad por defecto
int decimales = 3;                // Decimales
bool pesoEstable = false;         // Indicador de estabilidad
float capacidadMaxima = 80.0;     // Capacidad máxima (por defecto 80kg)
float capacidadIntermedia = 0;    // Capacidad intermedia para modos duales
int division = 1;                 // División (1, 2, 5)
int division2 = 1;                // División secundaria para dual interval

void setup() {
  Serial.begin(9600);
  balanza.begin(DT, SCK);

  // Configuración inicial de la balanza
  balanza.set_scale(factorCalibracion);
  balanza.tare(); // Cero inicial
}

void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim(); // Eliminar espacios
    procesarComando(comando);
  }

  // Leer peso y determinar su estado
  float peso = balanza.get_units(10) / 1000; // Kg
  pesoEstable = verificarEstabilidad(peso);

  // Mostrar peso en el formato especificado
  mostrarPeso(peso);
}

void procesarComando(String comando) {
  if (comando.equalsIgnoreCase("Z")) {
    balanza.tare();
    Serial.println("Tare.");
  } else if (comando.startsWith("SET DECI")) {
    decimales = comando.substring(9).toInt();
    if (decimales >= 1 && decimales <= 3) {
      Serial.print("deci:");
      Serial.print(decimales);
      Serial.println("RET:OK");
    } else {
      Serial.println("ERROR.");
    }
  } else if (comando.startsWith("SETWEIGHTUNIT")) {
    String nuevaUnidad = comando.substring(14);
    if (nuevaUnidad.equals("0")) {
      unidad = "kg";
      Serial.println("OK");
    } else if (nuevaUnidad.equals("4")) {
      unidad = "lb";
      Serial.println("OK");
    } else {
      Serial.println("ERROR.");
    }
  } else if (comando.startsWith("CALL")) {
    float pesoConocido = comando.substring(5).toFloat();
    calibrarConPeso(pesoConocido);
  } else if (comando.startsWith("SETMODE")) {
    int modo = comando.substring(8).toInt();
    if (modo >= 0 && modo <= 2) {
      Serial.println("RET:OK");
      configurarModo(modo);
    } else {
      Serial.println("ERROR.");
    }
  } else if (comando.startsWith("SETFULL")) {
    capacidadMaxima = comando.substring(8).toFloat();
    Serial.print("Capacidad máxima: ");
    Serial.print(capacidadMaxima);
    Serial.println(" RET:OK");
  } else if (comando.startsWith("SETMID")) {
    capacidadIntermedia = comando.substring(7).toFloat();
    Serial.print("Capacidad intermedia: ");
    Serial.print(capacidadIntermedia);
    Serial.println(" RET:OK");
  } else if (comando.startsWith("SETDIV")) {
    int div = comando.substring(7).toInt();
    if (div >= 0 && div <= 2) {
      division = (div == 0) ? 1 : (div == 1) ? 2 : 5;
      Serial.print("División: ");
      Serial.print(division);
      Serial.println(" RET:OK");
    } else {
      Serial.println("ERROR.");
    }
  } else if (comando.startsWith("SETDIV2")) {
    int div2 = comando.substring(8).toInt();
    if (div2 >= 0 && div2 <= 2) {
      division2 = (div2 == 0) ? 1 : (div2 == 1) ? 2 : 5;
      Serial.print("División secundaria: ");
      Serial.print(division2);
      Serial.println(" RET:OK");
    } else {
      Serial.println("ERROR.");
    }
  } else {
    Serial.println("Comando no reconocido.");
  }
}

bool verificarEstabilidad(float peso) {
  static float pesoAnterior = 0;
  float diferencia = abs(peso - pesoAnterior);
  pesoAnterior = peso;

  // Considerar estable si la diferencia es menor a 0.05 (ajustable)
  return diferencia < 0.05;
}

void mostrarPeso(float peso) {
  if (peso > capacidadMaxima) { // Si el peso excede el máximo soportado
    Serial.println("---OL---");
  } else {
    String estado = pesoEstable ? "ST NW +" : "UT NW +";

    // Convertir el peso si la unidad es libras
    float pesoConvertido = (unidad == "lb") ? peso * 2.2046 : peso;

    Serial.print(estado);
    Serial.print(String(pesoConvertido, decimales));
    Serial.print(" ");
    Serial.println(unidad);
  }
}

void configurarModo(int modo) {
  switch (modo) {
    case 0:
      Serial.println("Modo: Single Interval");
      break;
    case 1:
      Serial.println("Modo: Dual Range");
      break;
    case 2:
      Serial.println("Modo: Dual Interval");
      break;
  }
}

void calibrarConPeso(float pesoConocido) {
  if (pesoConocido > 0) {
    if (unidad == "lb") {
      pesoConocido /= 2.2046; // Convertir libras a kilogramos
    }

    float lectura = balanza.get_value(10) / 1000;
    factorCalibracion = lectura / pesoConocido;
    balanza.set_scale(factorCalibracion);
    Serial.print("OK.");
    Serial.println(factorCalibracion);
  }
}
