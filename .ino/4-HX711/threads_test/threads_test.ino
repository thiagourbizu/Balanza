#include "HX711.h"
#include "pico/multicore.h"

// Pines
#define DT1 16
#define DT2 19
#define DT3 20
#define DT4 22
#define SCK 26

HX711 hx1, hx2, hx3, hx4;

volatile float peso1 = 0, peso2 = 0, peso3 = 0, peso4 = 0;

// Calibración
float f1 = 124.53;
float f2 = 126.52;
float f3 = 124.81;
float f4 = 125.72;

String unidad = "kg";
int decimales = 3;
bool pesoEstable = false;

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  delay(3000);

  hx1.begin(DT1, SCK); hx1.set_scale(f1); hx1.tare();
  hx2.begin(DT2, SCK); hx2.set_scale(f2); hx2.tare();
  hx3.begin(DT3, SCK); hx3.set_scale(f3); hx3.tare();
  hx4.begin(DT4, SCK); hx4.set_scale(f4); hx4.tare();

  multicore_launch_core1(leerHXcore1);
}

// ================= LOOP =================
void loop() {
  peso1 = hx1.get_units(3);
  peso2 = hx2.get_units(3);

  float total = leerPesoTotal();
  pesoEstable = verificarEstabilidad(total);
  mostrarPeso(total);

  if (Serial.available()) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    procesarComando(comando);
  }

  delay(100);
}

// ================= CORE 1 =================
void leerHXcore1() {
  while (true) {
    peso3 = hx3.get_units(3);
    peso4 = hx4.get_units(3);
  }
}

// ================= FUNCIONES =================
float leerPesoTotal() {
  return (peso1 + peso2 + peso3 + peso4) / 1000.0;
}

bool verificarEstabilidad(float peso) {
  static float anterior = 0;
  float diff = fabs(peso - anterior);
  anterior = peso;
  return diff < 0.05;
}

void mostrarPeso(float peso) {
  if (peso > 280.0) {
    Serial.println("---OL---");
    return;
  }

  String estado = pesoEstable ? "ST NW +" : "UT NW +";
  float convertido = (unidad == "lb") ? peso * 2.2046 : peso;

  Serial.print(estado);
  Serial.print(convertido, decimales);
  Serial.print(" ");
  Serial.println(unidad);
}

void procesarComando(String cmd) {
  if (cmd.startsWith("U:")) {
    String nuevaUnidad = cmd.substring(2);
    nuevaUnidad.toLowerCase();
    if (nuevaUnidad == "kg" || nuevaUnidad == "lb") {
      unidad = nuevaUnidad;
      Serial.print("Unidad cambiada a ");
      Serial.println(unidad);
    }
  } else if (cmd.startsWith("D:")) {
    int nuevosDecimales = cmd.substring(2).toInt();
    if (nuevosDecimales >= 0 && nuevosDecimales <= 5) {
      decimales = nuevosDecimales;
      Serial.print("Decimales cambiados a ");
      Serial.println(decimales);
    }
  } else if (cmd.startsWith("C:")) {
    int celda = cmd.substring(2).toInt();
    calibrarCelda(celda);
  } else {
    Serial.println("Comando no válido.");
  }
}

void calibrarCelda(int celda) {
  Serial.print("Calibrando celda ");
  Serial.println(celda);
  Serial.println("Retire el peso y presione Enter...");
  while (!Serial.available()) delay(100);
  Serial.read(); // Consumir Enter

  if (celda == 1) { hx1.tare(); delay(500); Serial.println("Coloque 1 kg en celda 1 y presione Enter."); }
  else if (celda == 2) { hx2.tare(); delay(500); Serial.println("Coloque 1 kg en celda 2 y presione Enter."); }
  else if (celda == 3) { hx3.tare(); delay(500); Serial.println("Coloque 1 kg en celda 3 y presione Enter."); }
  else if (celda == 4) { hx4.tare(); delay(500); Serial.println("Coloque 1 kg en celda 4 y presione Enter."); }
  else { Serial.println("Número de celda no válido."); return; }

  while (!Serial.available()) delay(100);
  Serial.read(); // Consumir Enter

  float peso = 0;
  if (celda == 1) peso = hx1.get_units(10);
  else if (celda == 2) peso = hx2.get_units(10);
  else if (celda == 3) peso = hx3.get_units(10);
  else if (celda == 4) peso = hx4.get_units(10);

  float nuevoFactor = peso / 1.0;
  Serial.print("Nuevo factor calibración celda ");
  Serial.print(celda);
  Serial.print(": ");
  Serial.println(nuevoFactor, 2);

  if (celda == 1) { f1 = nuevoFactor; hx1.set_scale(f1); }
  else if (celda == 2) { f2 = nuevoFactor; hx2.set_scale(f2); }
  else if (celda == 3) { f3 = nuevoFactor; hx3.set_scale(f3); }
  else if (celda == 4) { f4 = nuevoFactor; hx4.set_scale(f4); }
}
