#include <Wire.h>

#define EEPROM_ADDR 0x50 // Dirección base típica de la 24LC01B
#define EEPROM_SIZE 128  // La 24LC01B tiene 128 bytes

void setup() {
  Serial.begin(9600);
  Wire.begin();

  Serial.println("Leyendo EEPROM 24LC01B en hexadecimal:\n");

  for (int i = 0; i < EEPROM_SIZE; i += 16) {
    Serial.print("0x");
    if (i < 16) Serial.print("0");
    Serial.print(i, HEX);
    Serial.print(": ");

    for (int j = 0; j < 16; j++) {
      byte data = leerEEPROM(i + j);
      if (data < 16) Serial.print("0");
      Serial.print(data, HEX);
      Serial.print(" ");
    }
    Serial.println();
    delay(20);
  }
}

void loop() {}

byte leerEEPROM(int direccion) {
  Wire.beginTransmission(EEPROM_ADDR);
  Wire.write((byte)(direccion)); // Dirección de 1 byte para 24LC01B
  Wire.endTransmission();

  Wire.requestFrom(EEPROM_ADDR, 1);
  if (Wire.available()) {
    return Wire.read();
  }
  return 0xFF; // Retorno por defecto si falla
}
