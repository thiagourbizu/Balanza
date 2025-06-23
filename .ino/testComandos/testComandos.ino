#include "HX711.h"

// Pines del HX711
#define DT 16
#define SCK 10

HX711 balanza; 

// Configuraciones iniciales
float factorCalibracion = 125.04365; // Factor de calibración
String unidad = "kg";             // Unidad por defecto
int decimales = 3;                // Decimales
bool pesoEstable = false;         // Indicador de estabilidad

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
  float peso = balanza.get_units(80)/1000;  // Kg
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
    if (decimales == 1 || decimales == 2 || decimales == 3) {
      Serial.print("OK");
      Serial.println(decimales);
    } else {
      Serial.println("ERROR.");
    }
  } else if (comando.startsWith("SETWEIGHTUNIT")) {
    String nuevaUnidad = comando.substring(14); // Extraer la nueva unidad
    //Serial.println(nuevaUnidad);
    // Comparar correctamente las cadenas usando equals
    if (nuevaUnidad.equals("0")) {
      unidad = "kg"; // Si la entrada es "0", asignar "kg"
      Serial.println("OK");
    } else if (nuevaUnidad.equals("4")) {
      unidad = "lb"; // Si la entrada es "4", asignar "lb"
      Serial.println("OK");
    } 
    else {
      Serial.println("ERROR."); // Entrada no válida
    }
  } else if (comando.startsWith("CALL")) {
    Serial.println("OK.");
    float pesoConocido = comando.substring(5).toFloat();
    Serial.println(pesoConocido);
    calibrarConPeso(pesoConocido);
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
  if (peso > 280.0) { // Si el peso excede el máximo soportado
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

void calibrarConPeso(float pesoConocido) {
  if (pesoConocido > 0) {
    balanza.tare();
    float lectura = balanza.get_units(80)/1000;
    Serial.println(lectura);
    factorCalibracion = lectura / pesoConocido;
    balanza.set_scale(factorCalibracion);
    Serial.print("OK.");
    Serial.println(factorCalibracion);
  }
}
