#include <HX711.h>

#define DT 16  // DT de HX711 a pin digital 9
#define SCK 10 // SCK de HX711 a pin digital 

#define DT2 9  // DT de HX711 a pin digital 9
#define SCK2 8 // SCK de HX711 a pin digital 88

HX711 celda; // Crea objeto HX711
HX711 celda2; // Crea objeto HX711

float factorCalibracion = 1.0; // Factor inicial, ajustable

void setup() {
  Serial.begin(115200);
  celda.begin(DT, SCK);
  celda2.begin(DT2, SCK2);

  Serial.println("Escribe 'iniciar' en el monitor serie para empezar la calibración.");

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
  Serial.println("En 5 segundos inicia la calibración...");
  delay(500);

  celda.set_scale(); // Establece el factor de escala por defecto
  celda.tare();      // Realiza la tara

  celda2.set_scale(); // Establece el factor de escala por defecto
  celda2.tare();      // Realiza la tara

  Serial.println("Coloca un peso conocido y escribe su valor en gramos:");

  // Espera el peso conocido
  for (int i = 1; i <= 2; i++) {
    Serial.print("Iniciando calibración para celda ");
    Serial.println(i);
    
    while (true) {
        if (Serial.available() > 0) {
            String pesoConocidoStr = Serial.readStringUntil('\n');
            pesoConocidoStr.trim();
            float pesoConocido = pesoConocidoStr.toFloat();

            if (pesoConocido > 0) {
                float lectura = celda.get_value(10); // Realiza una única lectura
                float factorCalibracion = lectura / pesoConocido;
                if (i == 1) {
                  lectura = celda.get_value(10); // Para la celda 1
                  celda.set_scale(factorCalibracion);
                } else if (i == 2) {
                  lectura = celda2.get_value(10); // Para la celda 2
                  celda2.set_scale(factorCalibracion);
                }

                Serial.print("Factor de calibración calculado para celda ");
                Serial.print(i);
                Serial.print(": ");
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
}

void loop() {
  // Lectura de peso con el factor de calibración aplicado
  float peso1 = celda.get_units(10);
  float peso2 = celda2.get_units(10);
  // Calcular peso total
  float pesoTotal = peso1 + peso2;

  // Mostrar resultados
  Serial.print("Peso Celda 1: ");
  Serial.print(peso1, 2);
  Serial.print(" g, Peso Celda 2: ");
  Serial.print(peso2, 2);
  Serial.print(" g, Peso Total: ");
  Serial.print(pesoTotal, 2);
  Serial.println(" g");

  delay(200);
  

 
}
