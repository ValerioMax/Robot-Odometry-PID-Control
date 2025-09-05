gli oggetti BluetoothSerial vengono trattati ugualmente agli oggetti Serial
(si può usare BluetoothSerial.read(), BluetoothSerial.println(), ...)

carica questo sketch sull'esp
connetti pc/cellulare all'esp via bluetooth
individua porta seriale, quando si connette all'esp il pc ne crea due:
    - outgoing port: pc si comporta da Client e esp da Server
    - incoming port: pc si comporta da Server e esp da Client
Per programmi come questo (programma risiede nell'esp (server) e pc ci si connette)
usare la outgoing port.
Aprire CuteCom, PuTTY o termios o quello che ti pare su questa porta seriale a 9600 baudrate.

Collega TX, RX dell'esp a RX, TX dell'arduino
Assicurati che arduino e esp abbiano lo stesso baudarate per comunicare sulla UART

NOTA:
Per inviare caratteri su PuTTY vai su Caterogory -> Terminal e seleziona una combinazione:
    - Local echo: Force on, Local line editing: Auto/Force off  --> appena scrivi carattere lo invia
    - Local echo: Force on, Local line editing: Force on        --> devi andare a capo per inviare caratteri scritti

