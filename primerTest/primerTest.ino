#include "HX711.h" // incluye libreria HX711

#define DT 5  // DT de HX711 a pin digital 5
#define SCK 6 // SCK de HX711 a pin digital 6

HX711 celda; // crea objeto con nombre celda

float factorCalibracion = 1.0; // Factor inicial, será ajustado durante la calibración

void setup() {
  Serial.begin(9600); // inicializa monitor serie a 9600 baudios

  celda.begin(DT, SCK); // inicializa objeto con los pines a utilizar

  Serial.println("Escribe 'iniciar' en el monitor serie para empezar la calibración.");

  // Espera hasta que el usuario ingrese "iniciar" en el monitor serie
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

  // Proceso de calibración
  Serial.println("En 5 segundos arranca a calibrar."); // texto estático descriptivo
  delay(5000);
  celda.set_scale(); // establece el factor de escala por defecto
  celda.tare();      // realiza la tara o puesta a cero
  Serial.println("Coloca un peso conocido y escribe su valor en gramos:");

  // Espera hasta que el usuario ingrese el peso conocido
  while (true) {
    if (Serial.available() > 0) {
      String pesoConocidoStr = Serial.readStringUntil('\n');
      pesoConocidoStr.trim(); // Elimina espacios en blanco
      float pesoConocido = pesoConocidoStr.toFloat();

      if (pesoConocido > 0) {
        float lectura = celda.get_value(10); // Promedia 10 lecturas
        factorCalibracion = lectura / pesoConocido; // Calcula el factor de calibración
        celda.set_scale(factorCalibracion); // Configura el nuevo factor de calibración

        Serial.print("Factor de calibración calculado: ");
        Serial.println(factorCalibracion);
        Serial.println("Calibración completa.");
        break;
      } else {
        Serial.println("Valor inválido. Escribe un número mayor a 0.");
      }
    }
  }
}

void loop() {
  // Lectura de peso
  if (celda.is_ready()) {
    float pesoActual = celda.get_units(10); // Promedia 10 lecturas
    Serial.print("Peso actual: ");
    Serial.print(pesoActual);
    Serial.println(" gramos");
  } else {
    Serial.println("Balanza no está lista. Verifica las conexiones.");
  }

  delay(500); // Lee cada medio segundo
}
