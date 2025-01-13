#include "HX711.h" // incluye libreria HX711

#define DT1 8  // DT de HX711 a pin digital 5
#define SCK1 9 // SCK de HX711 a pin digital 6

#define DT2 10  // DT de HX711 a pin digital 5
#define SCK2 16 // SCK de HX711 a pin digital 6

HX711 celda1; // crea objeto con nombre celda
HX711 celda2; // crea objeto con nombre celda
void setup() {
  Serial.begin(9600); // inicializa monitor serie a 9600 baudios

  celda1.begin(DT1, SCK1); // inicializa objeto con los pines a utilizar
  celda2.begin(DT2, SCK2); // inicializa objeto con los pines a utilizar


  

  
}

void loop() {
  // Nada por aquí
  // Proceso de calibración
  celda1.set_scale(); // establece el factor de escala por defecto
  celda1.tare();      // realiza la tara o puesta a cero
  celda2.set_scale(); // establece el factor de escala por defecto
  celda2.tare();  
  Serial.print("Celda 1(barrita): "); // texto descriptivo
  Serial.println(celda1.get_units(20)); // obtiene valor promedio de 10 lecturas
  Serial.print("Celda 2:"); // texto descriptivo
  Serial.println(celda2.get_units(20)); // obtiene valor promedio de 10 lecturas
  
}
