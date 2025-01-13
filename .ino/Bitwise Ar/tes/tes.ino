#include <HX711.h>

#define DT 9     // DT de HX711 a pin digital 2
#define SCK 8    // SCK de HX711 a pin digital 3

HX711 celda;     // crea objeto con nombre celda
float peso =0;
void setup() {
  Serial.begin(9600);   // inicializa monitor serie a 9600 baudios
  Serial.println("Balanza con celda de carga"); // texto descriptivo

  celda.begin(DT, SCK);   // inicializa objeto con los pines a utilizar
  celda.set_scale(-43.97);  // establece el factor de escala obtenido del primer programa
  celda.tare();     // realiza la tara o puesta a cero
}

void loop() {
  if (Serial.available() > 0) {  // verifica si hay datos disponibles en el monitor serie
    char input = Serial.read();  // lee el carácter ingresado

    if (input == 't') {          // si el carácter es 't'
      celda.tare();              // realiza la tara
      Serial.println("TARE");
    }
  }

  peso = celda.get_units(10), 1;
  
  // Mostrar el peso en gramos
  Serial.print("Valor (gramos): ");
  Serial.print(peso);  // muestra el valor promedio de 10 lecturas
  Serial.println(" gramos");
  
  celda.power_down();  // apaga el módulo HX711
  delay(500);         // demora de 1 segundo
  celda.power_up();    // despierta el módulo HX711
}
