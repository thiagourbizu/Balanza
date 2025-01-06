#include "HX711.h"    // incluye libreria HX711

#define DT 5      // DT de HX711 a pin digital 2
#define SCK 6     // SCK de HX711 a pin digital 3

HX711 celda;      // crea objeto con nombre celda

void setup() {
  Serial.begin(9600);   // inicializa monitor serie a 9600 baudios
  Serial.println("Balanza con celda de carga"); // texto descriptivo

  celda.begin(DT, SCK);   // inicializa objeto con los pines a utilizar

  celda.set_scale(1.0);  // establece el factor de escala obtenido del primer programa
  celda.tare();     // realiza la tara o puesta a cero
}
void loop() {
   Serial.println("Hola!");
   while (true) {
    if (Serial.available() > 0) {
      String comando = Serial.readStringUntil('\n'); // Lee el comando ingresado
      comando.trim(); // Elimina espacios en blanco y saltos de línea
      if (comando.equalsIgnoreCase("iniciar")) {
        Serial.println("Comando recibido. Iniciando calibración...");
        break; // Sale del bucle si el comando es válido
      } else {
        Serial.println("Comando no reconocido. Escribe 'iniciar' para continuar.");
      }
    }
  }
  Serial.println("Colocar un peso conocido (10 seg)."); // texto estático descriptivo
  delay(10000); // demora de 10 segundos para colocar el peso conocido
  Serial.println(celda.get_units(10)); // obtiene valor promedio de 10 lecturas
  Serial.println("Hecho. Divide el valor mostrado por el peso colocado."); // texto descriptivo
}
