gli oggetti BluetoothSerial vengono trattati ugualmente agli oggetti Serial
(si può usare BluetoothSerial.read(), BluetoothSerial.println(), ...)

carica questo sketch sull'esp
connetti pc/cellulare all'esp via bluetooth
individua porta seriale, quando si connette all'esp il pc ne crea due:
    - outgoing port: pc si comporta da Client e esp da Server
    - incoming port: pc si comporta da Server e esp da Client
Per programmi come questo (programma risiede nell'esp (server) e pc ci si connette)
usare la outgoing port.
Aprire CuteCom, PuTTY o termios o quello che ti pare su questa porta seriale.


PER IL DEBUG CON CUTECOM (o altro):
- APRI PRIMA COMUNICAZIONE ALLA PORTA SERIAL E POI A QUELLA BLUETOOTH !
- Su Linux, col metodo linea di comando, si può mettere qualsiasi baudrate per la porta Bluetooth (tanto è virtuale) 


Collega TX, RX dell'esp a RX, TX dell'arduino
Assicurati che arduino e esp abbiano lo stesso baudarate per comunicare sulla UART

NOTA:
Per inviare caratteri su PuTTY vai su Caterogory -> Terminal e seleziona una combinazione:
    - Local echo: Force on, Local line editing: Auto/Force off  --> appena scrivi carattere lo invia
    - Local echo: Force on, Local line editing: Force on        --> devi andare a capo per inviare caratteri scritti

Per connettersi al Bluetooth da VM, su Virtual Box:
1. vai su Impostazioni -> USB
2. aggiungi controller usb bluetooth
3. avvia VM

Per connettersi a Bluetooth su Linux da linea di comando:
1. da terminale:
        bluetoothctl
2. nel prompt di bluetoothctl scrivi:
        scan on
3. trova il device, copia il suo MAC Address e fai paring e trusting:
        pair XX:XX:XX:XX:XX:XX
        trust XX:XX:XX:XX:XX:XX
4. usa il servizio SSP (Serial Port Profile) per creare una porta seriale virtuale che mappa
   la connesione bluetooth:
        sudo sdptool browse XX:XX:XX:XX:XX:XX
5. cerca servizio "Serial Port" e copia il channel number (un piccolo intero tipicamente).
   Poi crea effettivamente la porta seriale virtuale:
        sudo rfcomm bind /dev/rfcomm0 XX:XX:XX:XX:XX:XX <channel_number>
    [ crea il device file rfcomm0 (si può usare anche rfcomm1,2,...) ]

Ora il device è accessibile attraverso il device-file /dev/rfcomm0 (come le tty o ttyACM)
e ci si può interagire con CuteCom o altro

NOTA: se non si connette può essere un problema di permessi:
1. aggiungi user al gruppo che può usare rfcomm0
        sudo usermod -a -G dialout valerio
2. riavvia pc