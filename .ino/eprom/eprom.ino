#include <Wire.h>

#define EEPROM_I2C_ADDRESS 0x50

float leerFloatDesdeEEPROM(int startAddress) {
  byte data[4];
  for (int i = 0; i < 4; i++) {
    Wire.beginTransmission(EEPROM_I2C_ADDRESS);
    Wire.write(startAddress + i);
    Wire.endTransmission();
    Wire.requestFrom(EEPROM_I2C_ADDRESS, 1);
    if (Wire.available()) {
      data[i] = Wire.read();
    }
  }
  float valor;
  memcpy(&valor, data, sizeof(float));
  return valor;
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  float rawFloat = leerFloatDesdeEEPROM(0x12);
  float calibracion = rawFloat / 3.0355;//3.034551413001956;
  Serial.print("Factor de calibracion (raw float): ");
  Serial.println(rawFloat, 3);
  Serial.print("Factor de calibracion: ");
  Serial.println(calibracion, 3);
}
//3,033162857599489‬ a
//3,010866959648422‬ c
//3,035939968404423‬ b

/*
 con este factor 3.034551413001956
 A=126.658 (384,350)
 127,86765
 126,61834953055509800691813539779‬
 
 B=125.083 (379,570)
 125,04365014000988305056827540768‬
 124,35295
3,0523602375335687653569939434489‬

 C=124.173 (376,810)
 124,1344094877285455443913688025


 */
void loop() {
}
