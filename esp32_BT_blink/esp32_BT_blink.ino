#include "BluetoothSerial.h"

BluetoothSerial serialBT;
char c;

void setup() {
  serialBT.begin("Esp32-BT");
  pinMode(12, OUTPUT);
}

void loop() {
  serialBT.println("AO!!");
  if (serialBT.available())
    c = serialBT.read();

  if (c == '1')
    digitalWrite(12, HIGH);
  else if (c == '0')
    digitalWrite(12, LOW);
  
  delay(20);
}
