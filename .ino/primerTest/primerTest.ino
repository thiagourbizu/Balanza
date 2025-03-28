#include <HX711.h>

#define DT 16  // DT9
#define SCK 10 // SCK8

HX711 celda;

float factorCalibracion = 1.0; // Factor inicial, ajustable

void setup() {
  Serial.begin(9600);
  celda.begin(DT, SCK);

  Serial.println("Escribe 'i' en el monitor serie para empezar la calibración.");

  // Espera el comando "iniciar"
  while (true) {
    if (Serial.available() > 0) {
      String comando = Serial.readStringUntil('\n');
      comando.trim();
      if (comando.equalsIgnoreCase("i")) {
        Serial.println("Comando recibido. Iniciando calibración...");
        break;
      } else {
        Serial.println("Comando no reconocido. Escribe 'iniciar' para continuar.");
      }
    }
  }

  // Proceso de calibración
  Serial.println("En 5 milisegundos inicia la calibración...");
  delay(500);

  celda.set_scale(); 
  celda.tare();      //tare

  Serial.println("Coloca un peso conocido y escribe su valor en gramos:");
 
  // Espera el peso conocido
  while (true) {
    if (Serial.available() > 0) {
      String pesoConocidoStr = Serial.readStringUntil('\n');
      pesoConocidoStr.trim();
      float pesoConocido = pesoConocidoStr.toFloat();

      if (pesoConocido > 0) {
        float lectura = celda.get_value(10);
        factorCalibracion = lectura / pesoConocido;

        Serial.print("Factor de calibración calculado: ");
        Serial.println(factorCalibracion);
        Serial.println("Calibración completa.");
        celda.set_scale(factorCalibracion); // Aplica el factor de calibración
        break;
      } else {
        Serial.println("Valor inválido. Escribe un número mayor a 0.");
      }
    }
  }
}

void loop() {
  // Lectura de peso con el factor de calibración aplicado
  float peso = celda.get_units(10);
  Serial.print("Peso: ");
  Serial.print(peso);
  Serial.println(" g");

 
}
