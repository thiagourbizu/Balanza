#include <HX711.h>


// Pines de cada HX711
#define DT 13
#define DT2 8
#define DT3 7
#define DT4 5
#define SCK 6  // Mismo SCK para todos

HX711 celda; // Crea objeto HX711
HX711 celda2; // Crea objeto HX711

float factorCalibracion = 1.0; // Factor inicial, ajustable

void setup() {
  Serial.begin(9600);
  celda.begin(DT3, SCK);
  celda2.begin(DT2, SCK);

  Serial.println("Escribe 'iniciar' en el monitor serie para empezar la calibración.");

  celda.set_scale(); // Establece el factor de escala por defecto
  celda.tare();      // Realiza la tara

  celda2.set_scale(); // Establece el factor de escala por defecto
  celda2.tare();      // Realiza la tara

  Serial.println("Coloca un peso conocido y escribe su valor en gramos:");

  }

void loop() {
  // Lectura de peso con el factor de calibración aplicado
  float peso1 = celda.get_units(3)/1000;
  float peso2 = celda2.get_units(3)/1000;
  // Calcular peso total
  float pesoTotal = peso1 + peso2;

  // Mostrar resultados
  Serial.print("Peso Celda 1: ");
  Serial.print(peso1, 3);
  Serial.print(" kg, Peso Celda 2: ");
  Serial.print(peso2, 3);
  Serial.print(" kg, Peso Total: ");
  Serial.print(pesoTotal, 3);
  Serial.println(" kg");

  delay(200);
  

 
}
