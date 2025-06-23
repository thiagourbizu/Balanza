#include "HX711.h"
#include "pico/multicore.h"

// Pines
#define DT1 16
#define SCK1 10

#define DT2 19
#define SCK2 17

#define DT3 20
#define SCK3 18

#define DT4 22
#define SCK4 26

#define N 20
#define THRESHOLD_GRAMS 4.0

HX711 hx1, hx2, hx3, hx4;

volatile float lecturaCore0 = 0;
volatile float lecturaCore1 = 0;
float offset = 0;

// Activación individual
bool active1 = true;
bool active2 = true;
bool active3 = true;
bool active4 = false;

// Factores de calibración
float factor[4] = {126.658, 125.083, 124.173, 125.0}; //A B C D

void setup() {
  Serial.begin(115200);

  hx1.begin(DT1, SCK1); hx1.set_scale(factor[0]); 
  hx2.begin(DT2, SCK2); hx2.set_scale(factor[1]);
  hx3.begin(DT3, SCK3); hx3.set_scale(factor[2]);
  hx4.begin(DT4, SCK4); hx4.set_scale(factor[3]);

  tareGeneral();
  multicore_launch_core1(readCore1);
}

void loop() {

  float total = 0;
  for (int i = 0; i < N; i++) {
    total += hx1.get_units(1);
    total += hx2.get_units(1);
    }
  float avg = total / (N * 3.0);
  avg -= offset;
  if (avg < THRESHOLD_GRAMS && avg > -THRESHOLD_GRAMS) avg = 0.0;
  lecturaCore0 = avg;

  float totalPeso = (lecturaCore0 + lecturaCore1) /1000.0;
  
  Serial.print(totalPeso, 3);
  Serial.print(" kg - ");
  Serial.print(active1);
  Serial.print(active2);
  Serial.print(active3);
  Serial.println(active4);
  
  if (Serial.available()) {
    char comando = Serial.read();

    switch (comando) {
      case 'Z':
        tareGeneral();
      case 'z':
        tareGeneral();
        break;
        
      case '1':
        active1 = !active1;
        Serial.print("CELDA 1 ");
        Serial.println(active1);
        break;

      case '2':
        active2 = !active2;
        Serial.print("CELDA 2 ");
        Serial.println(active2);
        break;

      case '3':
        active3 = !active3;
        Serial.print("CELDA 3 ");
        Serial.println(active3);
        break;

      case '4':
        active4 = !active4;
        Serial.print("CELDA 4 ");
        Serial.println(active4);
        break;
    }
  }
}

// CORE 1
void readCore1() {
    const int samples = N;
    while(true){
      float total = 0;
      for (int i = 0; i < samples; i++) {
          total += hx3.get_units(1);
          total += hx4.get_units(1);
          //delay(5);
      }
      float avg = total / (samples * 3.0);
      avg -= offset;
      if (avg < THRESHOLD_GRAMS && avg > -THRESHOLD_GRAMS) avg = 0.0;
      lecturaCore1 = avg;
    }
}

void tareGeneral() {
  if (active1) hx1.tare();
  if (active2) hx2.tare();
  if (active3) hx3.tare();
  if (active4) hx4.tare();
  Serial.println("Tare general realizado.");
}
