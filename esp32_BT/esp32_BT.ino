#include "BluetoothSerial.h"

BluetoothSerial serialBT;
String msg_bt;

void setup() {
  Serial.begin(19200);
  serialBT.begin("Esp32-BT");
}

void loop() {
  if (serialBT.available()) {
    char incoming_char = serialBT.read();

    // SE non intasa la comunicazione:
    // non farlo così, ma inviare costantemente anche se non è arrivato un nuovo messaggio
    // --> fare che quando arriva il newline sovrascrive il messaggio, ma nel mentre lo continua a spedire
    if (incoming_char == '\n' || incoming_char == '\r') {
      Serial.println(msg_bt);
      msg_bt = "";
    }
    else
      msg_bt += incoming_char;
  }

  delay(20);
}
