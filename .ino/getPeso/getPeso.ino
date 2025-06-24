#include <Arduino.h>
#include "HX711.h"
#include "pico/multicore.h"

// HX711
HX711 hx1;
HX711 hx2;
HX711 hx3;
HX711 hx4;

// Pines
#define DT1 16
#define SCK1 10

#define DT2 19
#define SCK2 17

#define DT3 20
#define SCK3 18

#define DT4 22
#define SCK4 26

// Calibración por celda
float factor[4] = {126.658, 124.173, 125.083, 124}; // A - B - C - D

// Estado de CELDAS
bool activeCells[4] = {true, true, true, false};

float offset = 0;
#define THRESHOLD_GRAMS 3.5

// ====== Sincronización entre cores ======
volatile bool startSampling = false;
volatile bool core1_done = false;

volatile float core1_sampled_weight = 0.0;
volatile int core1_sampled_active_count = 0;

volatile int sample_count = 15;
// =========================================

void core1_entry() {
    while (true) {
        if (startSampling) {
            float subtotal = 0.0;
            int active1 = 0;
            for (int i = 0; i < sample_count; i++) {
                if (activeCells[2]) {
                    subtotal += hx3.get_units(1);
                    if (i == 0) active1++;
                }

                if (activeCells[3]) {
                    subtotal += hx4.get_units(1);
                    if (i == 0) active1++;
                }
                //Serial.println(i);
                delay(1); // Control de carga
            }
            
            core1_sampled_weight = subtotal;
            core1_sampled_active_count = active1;
            
            core1_done = true;
            startSampling = false;
        }

        delay(1); // Reduce CPU uso mientras espera
    }
}

float getStableWeight(int samples = 15) {
    float total = 0;
    int activeCount = 0;

    // Iniciar proceso en core1
    sample_count = samples;
    core1_done = false;
    startSampling = true;
    // Realiza lecturas en core0
    for (int i = 0; i < samples; i++) {
        if (activeCells[0]) {
            total += hx1.get_units(1);
            if (i == 0) activeCount++;
        }
        if (activeCells[1]) {
            total += hx2.get_units(1);
            if (i == 0) activeCount++;
        }
        //Serial.println(i);
    }
   
    // Esperar a que core1 termine
    while (!core1_done) {
        delay(1);
    }

    // Agregar resultados de core1
    total += core1_sampled_weight;
    activeCount += core1_sampled_active_count;

    if (activeCount == 0) return 0.0;

    float avg = total / (samples * activeCount);
    avg -= offset;

    if (avg < THRESHOLD_GRAMS && avg > -THRESHOLD_GRAMS) return 0.0;
    return avg;
}

void doTare() {
    offset = 0;
    float val = getStableWeight(20);
    offset = val;
    Serial.print("Tara realizada. Offset actual: ");
    Serial.print(offset / 1000.0, 3);
    Serial.println(" kg");
}

void setup() {
    Serial.begin(115200);

    // Inicializamos HX711
    hx1.begin(DT1, SCK1);
    hx2.begin(DT2, SCK2);
    hx3.begin(DT3, SCK3);
    hx4.begin(DT4, SCK4);

    hx1.set_scale(factor[0]);
    hx2.set_scale(factor[1]);
    hx3.set_scale(factor[2]);
    hx4.set_scale(factor[3]);

    hx1.tare();
    hx2.tare();
    hx3.tare();
    hx4.tare();

    // Lanzamos core 1
    multicore_launch_core1(core1_entry);
}

void loop() {
    float peso;
    peso = getStableWeight(15);
    Serial.print("Peso total: ");
    Serial.print(peso / 1000.0, 3);
    Serial.println(" kg"); 
    if (Serial.available()) {
        char cmd = Serial.read();
        if (cmd == 'W' || cmd == 'w') {
//            peso = getStableWeight(15);
//            Serial.print("Peso total: ");
//            Serial.print(peso / 1000.0, 3);
//            Serial.println(" kg");
        } else if (cmd == 'Z' || cmd == 'z') {
            doTare();
        }
    }
}

/*
  // Determinar el signo
  char signo = (peso < 0) ? '-' : '+';

  // Obtener valor absoluto para el formateo
  float valorAbsoluto = abs(peso);

  // Formatear la cadena
  // %06.3f: 6 dígitos enteros con ceros a la izquierda + . + 3 decimales
  // snprintf asegura que la cadena quede siempre bien
  snprintf(buffer, sizeof(buffer), "%c%09.3fkg", signo, valorAbsoluto);

  // Mostrar resultado
  Serial.println(buffer);
 */
