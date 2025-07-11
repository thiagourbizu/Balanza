#include "HX711.h"

#define DT_PIN 13
#define SCK_PIN 6

HX711 balanza;

float factor;  // Valor aproximado, lo ajustaremos
const int numLecturas = 25000;  // Número de lecturas para promediar

void setup() {
  Serial.begin(9600);
  Serial.println("Calibración de celda de carga con HX711");

  balanza.begin(DT_PIN, SCK_PIN);
  balanza.set_scale();  // No usar factor al principio
  balanza.tare();       // Tara inicial

  Serial.println("Retira el peso. Tarando...");
  delay(3000);          // Espera a que se estabilice
  balanza.tare();       // Toma el valor de tara

  Serial.println("Coloca el peso de referencia y presiona ENTER en el monitor serial");
}

void loop() {  if (Serial.available()) {
    Serial.println("Lest's Go.");
    Serial.read();  // Limpia el buffer
    // Promediar múltiples lecturas
    long lecturaTotal = 0;
    for (int i = 0; i < numLecturas; i++) {
      lecturaTotal += balanza.get_units(5);
      delay(1);
    }
    long lecturaPromedio = lecturaTotal / numLecturas;
    Serial.print("Lectura cruda promedio: ");
    Serial.println(lecturaPromedio);

    Serial.println("Introduce el valor del peso en gramos:");
    while (Serial.available() == 0);  // Esperar entrada
    float pesoConocido = Serial.parseFloat();

    // Calcular factor de calibración más preciso
    factor = lecturaPromedio / pesoConocido;
    Serial.print("Factor de calibración calculado: ");
    Serial.println(factor, 5);
  }
}
