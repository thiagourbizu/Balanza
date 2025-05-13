#include "HX711.h"

// Pines de cada HX711
#define DT1 7
#define DT2 13
#define DT3 8
#define DT4 5
#define SCK 6

#define iterations 10

HX711 hx711_1, hx711_2, hx711_3, hx711_4;

int factorCalibracion1 = 126.61834;
int factorCalibracion2 = 125.04365;
int factorCalibracion3 = 124.1344;
int factorCalibracion4 = 125.72;

unsigned long tiempoAnterior = 0;

bool active1 = false;
bool active2 = true;
bool active3 = false;
bool active4 = false;

String unidad = "kg";
int decimales = 3;
bool pesoEstable = false;

void setup() {
  //delay(3000);
  Serial.begin(9600);

  hx711_1.begin(DT1, SCK);
  hx711_2.begin(DT2, SCK);
  hx711_3.begin(DT3, SCK);
  hx711_4.begin(DT4, SCK);
  
  /*
  if (!hx711_1.is_ready()) Serial.println("Error: Celda de carga 1 no responde.");
  if (!hx711_2.is_ready()) Serial.println("Error: Celda de carga 2 no responde.");
  if (!hx711_3.is_ready()) Serial.println("Error: Celda de carga 3 no responde.");
  if (!hx711_4.is_ready()) Serial.println("Error: Celda de carga 4 no responde.");
  */
  
  hx711_1.set_scale(factorCalibracion1);
  hx711_2.set_scale(factorCalibracion2);
  hx711_3.set_scale(factorCalibracion3);
  hx711_4.set_scale(factorCalibracion4);

  hx711_1.tare();
  hx711_2.tare();
  hx711_3.tare();
  hx711_4.tare();
}

void loop() {
  if (Serial.available() > 0) {
    String comando = Serial.readStringUntil('\n');
    comando.trim();
    procesarComando(comando);
  }

  float pesoTotal = leerPesoTotal();
  mostrarPeso(pesoTotal);
}

float leerPesoTotal() {
  float peso = 0;

  if (active1) peso += hx711_1.get_units(iterations);
  if (active2) peso += hx711_2.get_units(iterations);
  if (active3) peso += hx711_3.get_units(iterations);
  if (active4) peso += hx711_4.get_units(iterations);

  return peso / 1000.0;  // Kg
}
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

  if (comando.startsWith("CALL ")) {
    // CALL X Y
    int indexEspacio1 = comando.indexOf(' ', 5);
    int indexEspacio2 = comando.indexOf(' ', indexEspacio1 + 1);

    if (indexEspacio1 > 0 && indexEspacio2 == -1) {
      // Extraer celda y peso
      int celda = comando.substring(5, indexEspacio1).toInt();
      float peso = comando.substring(indexEspacio1 + 1).toFloat();

      if (celda >= 1 && celda <= 4 && peso > 0) {
        calibrarCelda(celda, peso);
        Serial.print("OK CELDA ");
        Serial.print(celda);
        Serial.print(" = ");
        Serial.print(peso);
        Serial.println(" KG");
      } else {
        Serial.println("ERROR PARAMETROS CALL");
      }
    } else {
      Serial.println("ERROR FORMATO CALL. USA: CALL X Y");
    }
    return;
  }

  // Comando no reconocido
  Serial.println("ERROR COMANDO DESCONOCIDO");
}


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

  // Aplicar zona muerta de ±2.5g
  if (fabs(peso) < 0.0025) {
    peso = 0;
  }

  String estado = pesoEstable ? "ST NW " : "UT NW ";
  String signo = (peso >= 0) ? "+" : "-";
  String value = String(fabs((unidad == "lb") ? peso * 2.2046 : peso), decimales);
  String tiempoMs = String(tiempoTranscurrido) + "ms";

  Serial.println(estado + signo + value + unidad + " " + tiempoMs);
}
void calibrarCelda(int celda, float pesoConocido) {
  if (pesoConocido <= 0) return;

  long lectura = 0;
  float nuevoFactor = 0;

  switch (celda) {
    case 1:
      hx711_1.tare();
      Serial.println("Tare Realizado, colocar peso");
      delay(5000);
      lectura = hx711_1.get_value(50);
      nuevoFactor = lectura / pesoConocido;
      hx711_1.set_scale(nuevoFactor);
      factorCalibracion1 = nuevoFactor;
      break;

    case 2:
      hx711_2.tare();
      Serial.println("Tare Realizado, colocar peso");
      delay(5000);
      lectura = hx711_2.get_value(50);
      nuevoFactor = lectura / pesoConocido;
      hx711_2.set_scale(nuevoFactor);
      factorCalibracion2 = nuevoFactor;
      break;

    case 3:
      hx711_3.tare();
      Serial.println("Tare Realizado, colocar peso");
      delay(5000);
      lectura = hx711_3.get_value(50);
      nuevoFactor = lectura / pesoConocido;
      hx711_3.set_scale(nuevoFactor);
      factorCalibracion3 = nuevoFactor;
      break;

    case 4:
      hx711_4.tare();
      Serial.println("Tare Realizado, colocar peso");
      delay(5000);
      lectura = hx711_4.get_value(50);
      nuevoFactor = lectura / pesoConocido;
      hx711_4.set_scale(nuevoFactor);
      factorCalibracion4 = nuevoFactor;
      break;
  }

  Serial.print("Nuevo factor celda "); Serial.print(celda);
  Serial.print(": "); Serial.println(nuevoFactor);
}
