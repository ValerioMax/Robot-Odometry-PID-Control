#include "BluetoothSerial.h"

// check if Bluetooth is enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to enable it.
#endif

BluetoothSerial SerialBT;

const char* esp32_bt_name = "ESP32_UART_Bridge";

void setup() {
  Serial.begin(19200);
  Serial.println("ESP32 UART to Bluetooth Bridge");

  SerialBT.begin(esp32_bt_name);
  Serial.print("Bluetooth device '");
  Serial.print(esp32_bt_name);
  Serial.println("' is ready to pair");
}

void loop() {
  // receive data from Bluetooth and redirect with UART
  if (SerialBT.available()) {
    char data = SerialBT.read();
    Serial.write(data);
  }

  // receive data from UART and redirect with Bluetooth
  if (Serial.available()) {
    char data = Serial.read();
    SerialBT.write(data);
  }
}
