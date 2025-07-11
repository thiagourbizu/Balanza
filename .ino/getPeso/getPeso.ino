#include <Arduino.h>
#include "HX711.h"
#include "pico/multicore.h"

// HX711
HX711 hx1;
HX711 hx2;
HX711 hx3;
HX711 hx4;

// Pines Placa Changomas
//#define DT1 17
//#define SCK1 16
//
//#define DT2 19
//#define SCK2 18
//
//#define DT3 14
//#define SCK3 15
//
//#define DT4 13
//#define SCK4 12

// Pines placa actual 11-07
#define DT1 16
#define SCK1 15

#define DT2 18
#define SCK2 17

#define DT3 20
#define SCK3 19

#define DT4 22
#define SCK4 21

// Calibración celdas ABC
float factor[4] = {126.658, 125.083, 124.173, 126.658}; // A - B - C - D

//Chango mas (1-4, 2-3, 3-2, 4-1) 382.340 - 382.320 - 379.190 - 379.980
//float factor[4] = {125.956*0.25148514851485, 125.950*0.25148514851485, 124.918*0.25148514851485,125.179*0.25148514851485}; 

// Estado de CELDAS
bool activeCells[4] = {true, true, true, true};

float offset = 0;
#define THRESHOLD_GRAMS 3.5

// ====== Sincronización entre cores ======
volatile bool startSampling = false;
volatile bool core1_done = false;

volatile float core1_weight = 0.0;

volatile int sample_count = 15;
// =========================================

void core1_entry() {
  while (true) {
    if (startSampling) {
      float subtotal = 0.0;
      for (int i = 0; i < sample_count; i++) {
        if (activeCells[2]) subtotal += hx3.get_units(1);
        if (activeCells[3]) subtotal += hx4.get_units(1);
        //Serial.println(i);
        delay(1); // Control de carga
      }

      core1_weight = subtotal;

      core1_done = true;
      startSampling = false;
    }
    delay(1); // Reduce CPU uso mientras espera
  }
}

float getStableWeight(int samples = 15) {
  float total = 0;

  // Iniciar proceso en core1
  sample_count = samples;
  core1_done = false;
  startSampling = true;
  // Realiza lecturas en core0
  for (int i = 0; i < samples; i++) {
    if (activeCells[0]) total += hx1.get_units(1);
    if (activeCells[1]) total += hx2.get_units(1);
    //Serial.println(i);
    delay(1);
  }

  // Esperar a que core1 termine
  while (!core1_done) {
    delay(1);
  }

  // Agregar resultados de core1
  total += core1_weight;

  if (activeCells[0] == 0 && activeCells[1] == 0 && activeCells[2] == 0 && activeCells[3] == 0) return 0.0;

  float avg = total / samples;
  avg -= offset;

  if (avg < THRESHOLD_GRAMS && avg > -THRESHOLD_GRAMS) return 0.0;
  return avg; // Importante que entregue en .g
}

void doTare() {
  offset = 0;
  float val = getStableWeight(20);
  offset = val;
  Serial.println("+00000.000kg");
}

void setup() {
  Serial.begin(115200);

  // Inicializamos HX711
  hx1.begin(DT1, SCK1); hx1.set_scale(factor[0]);
  hx2.begin(DT2, SCK2); hx2.set_scale(factor[1]);
  hx3.begin(DT3, SCK3); hx3.set_scale(factor[2]);
  hx4.begin(DT4, SCK4); hx4.set_scale(factor[3]);

  // Lanzamos core 1
  multicore_launch_core1(core1_entry);
  // Tare Inicial
  doTare();
}

void loop() {
  if (Serial.available())
  {
    char serial = toupper(Serial.read());
    if (serial == 'W')
    {
      float peso = getStableWeight(20) / 1000;

      // Determinar el signo
      char signo = (peso < 0) ? '-' : '+';

      // Obtener valor absoluto para el formateo
      float valorAbsoluto = abs(peso);
      char buffer[13];

      // Formatear la cadena: +000012.345kg
      snprintf(buffer, sizeof(buffer), "%c%09.3fkg", signo, valorAbsoluto);
      Serial.println(buffer);
    }
    
    if (serial == 'A'){ 
      activeCells[0] = !activeCells[0];
      Serial.println(activeCells[0]);}
    if (serial == 'B'){
      activeCells[1] = !activeCells[1];
      Serial.println(activeCells[1]);}
    if (serial == 'C'){ 
      activeCells[2] = !activeCells[2];
      Serial.println(activeCells[2]);}
    if (serial == 'D'){ 
      activeCells[3] = !activeCells[3];
      Serial.println(activeCells[3]);}
      
    if (serial == 'Z') doTare();

  }
}
