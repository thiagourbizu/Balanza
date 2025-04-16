#include "HX711.h"

// Pines de cada HX711
#define DT1 13
#define DT2 8
#define DT3 7
#define DT4 5
#define SCK 6

HX711 hx711_1, hx711_2, hx711_3, hx711_4;

// Factores de calibración individuales
float factorCalibracion1 = 124.53;
float factorCalibracion2 = 126.52;
float factorCalibracion3 = 124.81;
float factorCalibracion4 = 125.72;

String unidad = "kg";
int decimales = 3;
bool pesoEstable = false;

void setup() {
  Serial.begin(9600);

  hx711_1.begin(DT1, SCK);
  hx711_2.begin(DT2, SCK);
  hx711_3.begin(DT3, SCK);
  hx711_4.begin(DT4, SCK);
  
  /*
  if (!hx711_1.is_ready()) Serial.println("Error: Celda de carga 1 no responde.");
  if (!hx711_2.is_ready()) Serial.println("Error: Celda de carga 2 no responde.");
  if (!hx711_3.is_ready()) Serial.println("Error: Celda de carga 3 no responde.");
  if (!hx711_4.is_ready()) Serial.println("Error: Celda de carga 4 no responde.");
  */
  
  hx711_1.set_scale(factorCalibracion1);
  hx711_2.set_scale(factorCalibracion2);
  hx711_3.set_scale(factorCalibracion3);
  hx711_4.set_scale(factorCalibracion4);

  hx711_1.tare();
  hx711_2.tare();
  hx711_3.tare();
  hx711_4.tare();
}

void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    procesarComando(comando);
  }

  float pesoTotal = leerPesoTotal();
  pesoEstable = verificarEstabilidad(pesoTotal);
  mostrarPeso(pesoTotal);
}

float leerPesoTotal() {
  float p1 = hx711_1.get_units(3);
  float p2 = hx711_2.get_units(3);
  float p3 = hx711_3.get_units(3);
  float p4 = hx711_4.get_units(3);

  return (p1 + p2 + p3 + p4) / 1000.0;  // Kg
}

void procesarComando(String comando) {
  if (comando.equalsIgnoreCase("Z")) {
    hx711_1.tare();
    hx711_2.tare();
    hx711_3.tare();
    hx711_4.tare();
    Serial.println("Tare.");
  } else if (comando.startsWith("SET DECI")) {
    decimales = comando.substring(9).toInt();
    if (decimales >= 1 && decimales <= 3) {
      Serial.print("OK");
      Serial.println(decimales);
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
    // Esperado: CALL X Y
    int indexEspacio1 = comando.indexOf(' ');
    int indexEspacio2 = comando.indexOf(' ', indexEspacio1 + 1);

    if (indexEspacio1 != -1 && indexEspacio2 != -1) {
      int celda = comando.substring(indexEspacio1 + 1, indexEspacio2).toInt();
      float peso = comando.substring(indexEspacio2 + 1).toFloat();

      if (celda >= 1 && celda <= 4 && peso > 0) {
        calibrarCelda(celda, peso);
        Serial.print("Celda "); Serial.print(celda);
        Serial.print(" calibrada con "); Serial.print(peso); Serial.println(" kg.");
      } else {
        Serial.println("ERROR en parámetros.");
      }
    } else {
      Serial.println("Formato incorrecto. Usa: CALL X Y");
    }
  } else {
    Serial.println("Comando no reconocido.");
  }
}

bool verificarEstabilidad(float peso) {
  static float pesoAnterior = 0;
  float diferencia = abs(peso - pesoAnterior);
  pesoAnterior = peso;
  return diferencia < 0.05;
}

void mostrarPeso(float peso) {
  if (peso > 280.0) {
    Serial.println("---OL---");
  } else {
    String estado = pesoEstable ? "ST NW +" : "UT NW +";
    float pesoConvertido = (unidad == "lb") ? peso * 2.2046 : peso;
    Serial.print(estado);
    Serial.print(String(pesoConvertido, decimales));
    Serial.print(" ");
    Serial.println(unidad);
  }
}

void calibrarCelda(int celda, float pesoConocido) {
  if (pesoConocido <= 0) return;

  long lectura = 0;
  float nuevoFactor = 0;

  switch (celda) {
    case 1:
      hx711_1.tare();
      Serial.println("Tare Realizado, colocar peso");
      delay(5000);
      lectura = hx711_1.get_value(10);
      nuevoFactor = lectura / pesoConocido;
      hx711_1.set_scale(nuevoFactor);
      factorCalibracion1 = nuevoFactor;
      break;

    case 2:
      hx711_2.tare();
      Serial.println("Tare Realizado, colocar peso");
      delay(5000);
      lectura = hx711_2.get_value(10);
      nuevoFactor = lectura / pesoConocido;
      hx711_2.set_scale(nuevoFactor);
      factorCalibracion2 = nuevoFactor;
      break;

    case 3:
      hx711_3.tare();
      Serial.println("Tare Realizado, colocar peso");
      delay(5000);
      lectura = hx711_3.get_value(10);
      nuevoFactor = lectura / pesoConocido;
      hx711_3.set_scale(nuevoFactor);
      factorCalibracion3 = nuevoFactor;
      break;

    case 4:
      hx711_4.tare();
      Serial.println("Tare Realizado, colocar peso");
      delay(5000);
      lectura = hx711_4.get_value(10);
      nuevoFactor = lectura / pesoConocido;
      hx711_4.set_scale(nuevoFactor);
      factorCalibracion4 = nuevoFactor;
      break;
  }

  Serial.print("Nuevo factor celda "); Serial.print(celda);
  Serial.print(": "); Serial.println(nuevoFactor);
}
