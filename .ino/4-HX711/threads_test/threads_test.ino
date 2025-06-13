#include "HX711.h"
#include "pico/multicore.h"

// Pines
#define DT1 16
#define DT2 19
#define DT3 20
#define DT4 22
#define SCK 26

#define iterations 10
HX711 hx1, hx2, hx3, hx4;

volatile float peso1 = 0, peso2 = 0, peso3 = 0, peso4 = 0;

// Factores de calibración
float factor[4] = {124.53,126.52,124.81,125.72};

void setup() {
  Serial.begin(115200);
  delay(3000);

  hx1.begin(DT1, SCK); hx1.set_scale(factor[0]); 
  hx2.begin(DT2, SCK); hx2.set_scale(factor[1]);
  hx3.begin(DT3, SCK); hx3.set_scale(factor[2]);
  hx4.begin(DT4, SCK); hx4.set_scale(factor[3]);

  tareGeneral();
  
  multicore_launch_core1(leerHXcore1);
}

void loop() {
  peso1 = hx1.get_units(iterations);
  peso2 = hx2.get_units(iterations);
  float total = (peso1 + peso2 + peso3 + peso4) / 1000.0;

  Serial.print("Peso total: ");
  Serial.print(total, 3);
  Serial.println(" kg");

  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'z' || cmd == 'Z') {
      tareGeneral();
    }
  }
}

void leerHXcore1() {
  while (true) {
    peso3 = hx3.get_units(iterations);
    peso4 = hx4.get_units(iterations);
  }
}
void tareGeneral() {
  hx1.tare();
  hx2.tare();
  hx3.tare();
  hx4.tare();
  Serial.println("Tare general realizado.");
}
