#include "HX711.h"

// Pines por celda
#define DT1 7
#define DT2 13
#define DT3 8
#define DT4 5
#define SCK 6

HX711 scale1;
HX711 scale2;
HX711 scale3;
HX711 scale4;
HX711* scales[] = { &scale1, &scale2, &scale3, &scale4 };

// Factores calibrados por celda
float scaleFactors[4] = { 126.658, 125.043650140, 124.173, 216.9 };  // en gramos

// Filtro y estabilidad
float pesoSuavizado = 0;
float pesoAnterior = 0;
float alpha = 0.5;  // Filtro exponencial
const float zonaMuerta = 5; // Menos de 5g se considera 0
const float umbralInestabilidad = 10; // Cambio mayor a 10g = inestable

bool esEstable = true;
int lecturasEstables = 0;
const int minLecturasEstables = 3;

void setup() {
  Serial.begin(9600);

  scale1.begin(DT1, SCK);
  scale2.begin(DT2, SCK);
  scale3.begin(DT3, SCK);
  scale4.begin(DT4, SCK);

  for (int i = 0; i < 4; i++) {
    scales[i]->set_scale(scaleFactors[i]);
    scales[i]->tare();
  }

  Serial.println("Listo para medir.");
}

void loop() {
  float peso = leerPesoTotal();

  // Filtro exponencial
  pesoSuavizado = alpha * peso + (1 - alpha) * pesoSuavizado;

  // Zona muerta
  if (abs(pesoSuavizado) < zonaMuerta) {
    pesoSuavizado = 0;
  }

  // Detección de inestabilidad
  float diferencia = abs(pesoSuavizado - pesoAnterior);
  if (diferencia > umbralInestabilidad) {
    esEstable = false;
    lecturasEstables = 0;
  } else {
    lecturasEstables++;
    if (lecturasEstables >= minLecturasEstables) {
      esEstable = true;
    }
  }

  pesoAnterior = pesoSuavizado;

  // Mostrar resultado
  Serial.print("Peso (g): ");
  Serial.print(pesoSuavizado);
  Serial.print(" - Estado: ");
  Serial.println(esEstable ? "Estable" : "INestable");
}

float leerPesoTotal() {
  float total = 0;
  int count = 0;
  for (int i = 0; i < 4; i++) {
    if (scales[i]->is_ready()) {
      total += scales[i]->get_units(10);
      count++;
    }
  }
  return (count > 0) ? total : 0; //cambiar a ==4 
}
