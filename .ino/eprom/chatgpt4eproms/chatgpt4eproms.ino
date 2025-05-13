#include <SoftwareWire.h>  // https://github.com/Testato/SoftwareWire

#define SDA1 A0
#define SDA2 A1
#define SDA3 A2
#define SDA4 A3
#define SCL  A4  // común a todas

#define EEPROM_I2C_ADDRESS 0x50

SoftwareWire eeprom1(SDA1, SCL);
SoftwareWire eeprom2(SDA2, SCL);
SoftwareWire eeprom3(SDA3, SCL);
SoftwareWire eeprom4(SDA4, SCL);

float leerFloatDesdeEEPROM(SoftwareWire &wire, int startAddress) {
  byte data[4];
  for (int i = 0; i < 4; i++) {
    wire.beginTransmission(EEPROM_I2C_ADDRESS);
    wire.write(startAddress + i);
    wire.endTransmission();
    wire.requestFrom(EEPROM_I2C_ADDRESS, 1);
    if (wire.available()) {
      data[i] = wire.read();
    }
  }
  float valor;
  memcpy(&valor, data, sizeof(float));
  return valor;
}

void setup() {
  Serial.begin(9600);

  // Iniciar los 4 buses
  eeprom1.begin();
  eeprom2.begin();
  eeprom3.begin();
  eeprom4.begin();

  float raw1 = leerFloatDesdeEEPROM(eeprom1, 0x12);
  float raw2 = leerFloatDesdeEEPROM(eeprom2, 0x12);
  float raw3 = leerFloatDesdeEEPROM(eeprom3, 0x12);
  float raw4 = leerFloatDesdeEEPROM(eeprom4, 0x12);

  float factor = 3.0355;

  Serial.println("== FACTORES DE CALIBRACIÓN ==");

  Serial.print("EEPROM A: ");
  Serial.print(raw1, 3);
  Serial.print(" => ");
  Serial.println(raw1 / factor, 3);

  Serial.print("EEPROM B: ");
  Serial.print(raw2, 3);
  Serial.print(" => ");
  Serial.println(raw2 / factor, 3);

  Serial.print("EEPROM C: ");
  Serial.print(raw3, 3);
  Serial.print(" => ");
  Serial.println(raw3 / factor, 3);

  Serial.print("EEPROM D: ");
  Serial.print(raw4, 3);
  Serial.print(" => ");
  Serial.println(raw4 / factor, 3);
}

void loop() {
  // Nada aquí por ahora
}
