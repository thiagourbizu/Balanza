 #include <HX711.h>

#define DT 16  // DT de HX711 a pin digital 9
#define SCK 10 // SCK de HX711 a pin digital 8

HX711 celda; // Crea objeto HX711

float peso =0;
void setup() {
  Serial.begin(115200);   // inicializa monitor serie a 9600 baudios
  Serial.println("Balanza con celda de carga"); // texto descriptivo

  celda.begin(DT, SCK);   // inicializa objeto con los pines a utilizar
  celda.set_scale(-23.95);  // establece el factor de escala obtenido del primer programa
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
  delay(200);         // demora de 1 segundo
  celda.power_up();    // despierta el módulo HX711
}
