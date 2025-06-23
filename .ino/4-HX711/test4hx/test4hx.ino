#include "HX711.h"

// Pines
#define DT1 16
#define DT2 19
#define DT3 20
#define SCK 26

#define iterations 15
HX711 hx1, hx2, hx3;

float peso1 = 0, peso2 = 0, peso3 = 0;
float peso1_suav = 0, peso2_suav = 0, peso3_suav = 0;

const float alpha = 0.95;  // filtro exponencial para suavizado (más peso a historial)
const float umbral = 3.0;  // en gramos = 0.003 kg

unsigned long lastMillis = 0;
float ultimoTotal = 0.0;

// Factores de calibración actualizados
float factor[3] = {126.658, 124.173, 125.083};

void setup() {
  Serial.begin(115200);
  delay(3000);

  hx1.begin(DT1, SCK); hx1.set_scale(factor[0]);
  hx2.begin(DT2, SCK); hx2.set_scale(factor[1]);
  hx3.begin(DT3, SCK); hx3.set_scale(factor[2]);

  tareGeneral();
  lastMillis = millis();

  // Inicializar suavizado con primeras lecturas
  peso1_suav = hx1.get_units(iterations);
  peso2_suav = hx2.get_units(iterations);
  peso3_suav = hx3.get_units(iterations);
}

void loop() {
  peso1 = hx1.get_units(iterations);
  peso2 = hx2.get_units(iterations);
  peso3 = hx3.get_units(iterations);

  peso1_suav = peso1_suav * alpha + peso1 * (1 - alpha);
  peso2_suav = peso2_suav * alpha + peso2 * (1 - alpha);
  peso3_suav = peso3_suav * alpha + peso3 * (1 - alpha);

  // suma en gramos, porque get_units da peso en "unidades", 
  // suponiendo factor calibrado para gramos (si no, ajustá aquí)
  float total = peso1_suav + peso2_suav + peso3_suav;

  // Aplicar umbral: si total dentro ±3 gramos, poner a 0
  if (abs(total) < umbral) {
    total = 0.0;
  }

  // Convertir a kg para impresión
  float totalKg = total / 1000.0;

  unsigned long now = millis();
  unsigned long delta = now - lastMillis;
  lastMillis = now;

  Serial.print("Peso total: ");
  Serial.print(totalKg, 3);
  Serial.print(" kg (Δt = ");
  Serial.print(delta);
  Serial.println(" ms)");

  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'z' || cmd == 'Z') {
      tareGeneral();
    }
  }
}

void tareGeneral() {
  hx1.tare();
  hx2.tare();
  hx3.tare();
  Serial.println("Tare general realizado.");
}
