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

#define iterations 10

HX711 hx711_1, hx711_2, hx711_3, hx711_4;

float lecturaCore0 = 0;
volatile float lecturaCore1 = 0;

int factorCalibracion1 = 125.083;
int factorCalibracion2 = 124.173;
int factorCalibracion3 = 126.658;
int factorCalibracion4 = 125;

unsigned long tiempoAnterior = 0;

bool active1 = false;
bool active2 = true;
bool active3 = true;
bool active4 = true;

String unidad = "kg";
int decimales = 3;
bool pesoEstable = false;

// ================= CORE 1 =================
void core1Loop() {
  while (true) {
    float peso = 0;
    if (active3) peso += hx711_3.get_units(iterations);
    if (active4) peso += hx711_4.get_units(iterations);
    lecturaCore1 = peso / 1000.0; // Convertir a kg
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);

  hx711_1.begin(DT1, SCK1);
  hx711_2.begin(DT2, SCK2);
  hx711_3.begin(DT3, SCK3);
  hx711_4.begin(DT4, SCK4);

  hx711_1.set_scale(factorCalibracion1);
  hx711_2.set_scale(factorCalibracion2);
  hx711_3.set_scale(factorCalibracion3);
  hx711_4.set_scale(factorCalibracion4);

  hx711_1.tare();
  hx711_2.tare();
  hx711_3.tare();
  hx711_4.tare();

  multicore_launch_core1(core1Loop);
}

// ================= LOOP =================
void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    procesarComando(comando);
  }

  // Leer celdas en este core
  float peso = 0;
  if (active1) peso += hx711_1.get_units(iterations);
  if (active2) peso += hx711_2.get_units(iterations);
  lecturaCore0 = peso / 1000.0;

  float pesoTotal = lecturaCore0 + lecturaCore1;
  mostrarPeso(pesoTotal);
}

// ================= COMANDOS =================
void procesarComando(String comando) {
  comando.trim();
  comando.toUpperCase();

  if (comando == "Z") {
    hx711_1.tare();
    hx711_2.tare();
    hx711_3.tare();
    hx711_4.tare();
    Serial.println("OK TARE");
    return;
  }

  if (comando == "1") {
    active1 = !active1;
    Serial.print("CELDA 1 ");
    Serial.println(active1 ? "ACTIVADA" : "DESACTIVADA");
    return;
  }

  if (comando == "2") {
    active2 = !active2;
    Serial.print("CELDA 2 ");
    Serial.println(active2 ? "ACTIVADA" : "DESACTIVADA");
    return;
  }

  if (comando == "3") {
    active3 = !active3;
    Serial.print("CELDA 3 ");
    Serial.println(active3 ? "ACTIVADA" : "DESACTIVADA");
    return;
  }

  if (comando == "4") {
    active4 = !active4;
    Serial.print("CELDA 4 ");
    Serial.println(active4 ? "ACTIVADA" : "DESACTIVADA");
    return;
  }

  if (comando.startsWith("SET DECI ")) {
    String valor = comando.substring(9);
    int nuevosDecimales = valor.toInt();
    if (nuevosDecimales >= 1 && nuevosDecimales <= 3) {
      decimales = nuevosDecimales;
      Serial.print("OK ");
      Serial.println(decimales);
    } else {
      Serial.println("ERROR DECIMALES");
    }
    return;
  }

  if (comando.startsWith("SETWEIGHTUNIT ")) {
    String valor = comando.substring(15);
    if (valor == "0") {
      unidad = "kg";
      Serial.println("OK UNIDAD KG");
    } else if (valor == "4") {
      unidad = "lb";
      Serial.println("OK UNIDAD LB");
    } else {
      Serial.println("ERROR UNIDAD");
    }
    return;
  }
  Serial.println("ERROR COMANDO DESCONOCIDO");
}


// ================= DISPLAY PESO =================
bool verificarEstabilidad(float peso) {
  static float pesoAnterior = 0;
  float diferencia = abs(peso - pesoAnterior);
  pesoAnterior = peso;
  return diferencia < 0.05;
}

void mostrarPeso(float peso) {
  unsigned long tiempoActual = millis();
  unsigned long tiempoTranscurrido = tiempoActual - tiempoAnterior;
  tiempoAnterior = tiempoActual;

  if (peso > 280.0) {
    Serial.println("---OL---");
    return;
  }

  // Zona muerta de ±2.5g
  if (fabs(peso) < 0.0025) peso = 0;

  pesoEstable = verificarEstabilidad(peso);

  String estado = pesoEstable ? "ST NW " : "UT NW ";
  String signo = (peso >= 0) ? "+" : "-";
  String value = String(fabs((unidad == "lb") ? peso * 2.2046 : peso), decimales);
  String tiempoMs = String(tiempoTranscurrido) + "ms";

  Serial.println(estado + signo + value + unidad + " " + tiempoMs);
}
