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

// #include "BluetoothSerial.h"

// BluetoothSerial serialBT;
// String msg_bt;
// String msg_uart;

// void setup() {
//   Serial.begin(19200);
//   serialBT.begin("Esp32-BT");

//   Serial.println("Starting communication UART");
//   serialBT.println("Starting communication BT");
// }

// void loop() {
//   serialBT.println("BT");
 
//   if (serialBT.available() > 0) { // ritorna il numero di byte available da leggere
//     char recv_byte = serialBT.read();
  
//     if (recv_byte == '\n' || recv_byte == '\r') {
//       Serial.println(msg_bt);
//       msg_bt = "";
//     }
//     else
//       msg_bt += recv_byte;
//   }

//   if (Serial.available() > 0) { // ritorna il numero di byte available da leggere
//     char recv_byte = Serial.read();
  
//     if (recv_byte == '\n' || recv_byte == '\r') {
//       serialBT.println(msg_uart);
//       msg_uart = "";
//     }
//     else
//       msg_uart += recv_byte;
//   }
  
//   delay(20);
// }
