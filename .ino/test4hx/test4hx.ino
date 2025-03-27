#include "HX711.h"

// Pines de cada HX711
#define DT1 13
#define DT2 8
#define DT3 7
#define DT4 5
#define SCK 6  // Mismo SCK para todos

float factor_calibracion = 107.4;
HX711 hx711_1, hx711_2, hx711_3, hx711_4;

// Declaración de funciones
void tarearBalanza();
void calibrarBalanza(float peso_referencia);

void setup() {
  Serial.begin(9600);

  hx711_1.begin(DT1, SCK);
  hx711_2.begin(DT2, SCK);
  hx711_3.begin(DT3, SCK);
  hx711_4.begin(DT4, SCK);

  Serial.println("Verificando sensores...");
  bool error = false;

  if (!hx711_1.is_ready()) {
    Serial.println("Error: Celda de carga 1 no responde.");
    error = true;
  }
  if (!hx711_2.is_ready()) {
    Serial.println("Error: Celda de carga 2 no responde.");
    error = true;
  }
  if (!hx711_3.is_ready()) {
    Serial.println("Error: Celda de carga 3 no responde.");
    error = true;
  }
  if (!hx711_4.is_ready()) {
    Serial.println("Error: Celda de carga 4 no responde.");
    error = true;
  }

  if (error) {
    while (1);  // Detener ejecución si hay error
  }

  Serial.println("HX711 Listos. Tarando...");
 
  hx711_1.tare();
  hx711_2.tare();
  hx711_3.tare();
  hx711_4.tare();
 
  delay(1000);  // Esperar estabilización

  hx711_1.set_scale(factor_calibracion);
  hx711_2.set_scale(factor_calibracion);
  hx711_3.set_scale(factor_calibracion);
  hx711_4.set_scale(factor_calibracion);
}

void loop() {
  // Leer cada celda de carga en gramos
  float peso1 = hx711_1.get_units(3)/1000;
  float peso2 = hx711_2.get_units(3)/1000;
  float peso3 = hx711_3.get_units(3)/1000;
  float peso4 = hx711_4.get_units(3)/1000;

  float peso_total = peso1 + peso2 + peso3 + peso4;

  Serial.print("Peso 1: "); Serial.print(peso1, 3); Serial.print(" kg | ");
  Serial.print("Peso 2: "); Serial.print(peso2, 3); Serial.print(" kg | ");
  Serial.print("Peso 3: "); Serial.print(peso3, 3); Serial.print(" kg | ");
  Serial.print("Peso 4: "); Serial.print(peso4, 3); Serial.print(" kg | ");
  Serial.print("Peso Total: "); Serial.print(peso_total, 3); Serial.println(" kg");

  // Revisar si hay comando desde el monitor serie
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.equalsIgnoreCase("Z")) {
      tarearBalanza();
    }
    else if (input.startsWith("CALL ")) {
      float pesoConocido = input.substring(5).toFloat();
      if (pesoConocido > 0) {
        calibrarBalanza(pesoConocido);
      } else {
        Serial.println("Error: Formato incorrecto. Use 'CALL <peso>'");
      }
    } else {
      Serial.println("Error: Comando no válido. Use 'CALL <peso>' o 'Z'");
    }
  }
}

void calibrarBalanza(float peso_referencia) {
  Serial.println("Calibrando... No toques la balanza.");
  Serial.print("Peso de referencia: "); Serial.print(peso_referencia); Serial.println(" g");

  // Realizar tare en todas las celdas
  hx711_1.tare();
  hx711_2.tare();
  hx711_3.tare();
  hx711_4.tare();


  Serial.println("Coloca el peso de referencia sobre la balanza y presiona Enter cuando esté listo.");
 
  while (!Serial.available());  // Esperar confirmación
  Serial.read();

  float lectura = hx711_1.get_units(10) + hx711_2.get_units(10) + hx711_3.get_units(10) + hx711_4.get_units(10);
  float nuevo_factor = lectura / peso_referencia;

  hx711_1.set_scale(nuevo_factor);
  hx711_2.set_scale(nuevo_factor);
  hx711_3.set_scale(nuevo_factor);
  hx711_4.set_scale(nuevo_factor);

  Serial.print("Nuevo factor de calibración: ");
  Serial.println(nuevo_factor, 5);
  Serial.println("Calibración completada.");
}

void tarearBalanza() {
  Serial.println("Realizando Tare... Espere.");
  hx711_1.tare();
  hx711_2.tare();
  hx711_3.tare();
  hx711_4.tare();
  Serial.println("Tare completado.");
}
