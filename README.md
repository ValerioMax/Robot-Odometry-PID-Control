# Robot-Odometry-PID-Control

Mappare porta seriale su virtual machine (VirtualBox):
vai su Impostazioni -> PorteSeriali,
abilita porta seriale,
puoi fare fino a 4 mapping: selezionane una (es: COM1)
Modalità porta: Pipe host,
checkka "Connetti alla pipe/socket esistente",
Su Percorso/Indirizzo inserisci la porta della macchina host (porta da mappare) (es: su Windows se è COM10 --> \\.\COM10)
Ora la porta verrà mappata sulla porta COM1 della macchina guest (macchina virtuale) (c'è scritto COM1 ma se la macchina è Linux la si leggerà come ttyS0),
riavvia macchina virtuale.

NOTA:
se si interrompe la connessione bluetooth (ad esempio rebootando l'esp quando si riflasha il codice su di esso)
poi la connessione bluetooth si ricrea automaticamente ma la pipe si è rotta e quindi sulla porta mappata sulla VM non si leggerà
più nulla.
Bisogna prima caricare il codice sull'esp e poi startare l'intera VM per evitarlo.


Mappare dispositivo USB su virtual machine (VirtualBox):
vai su Impostazioni -> USB,
aggiungi nuovo filtro USB (icona +) selezionando il dispositivo,
riavvia macchina virtuale.

-------------------------------

Ricordati di creare la cartella build/ quando cloni da github

-------------------------------

NON usare malloc nei microcontrollori, è meglio usare variabili globali oppure variabili locali passate alle funzioni deferenziandole
(per oggetti usati nelle ISR bisogna perforza dichiarare l'oggetto globalmente perché non si possono passare parametri)

-------------------------------

le classi sono leggermente hardcoded: supponiamo che ogni oggetto Motor abbia in1_pin e in2_pin nella porta A e usi il TIMER4 per pwm_pin
--> si potrebbe rendere generale usando es:
        *(dir_port_register) invece che PORTA
    dove la funzione prende &PORTA come param in input: void foo(..., volatile uint8_t *dir_port_register, ...) {}
Tutti gli switch case sono un tentativo di generalizzare un minimo in modo tale da poter usare i vari metodi/funzioni su più pin/periferiche diverse

-------------------------------

per i linking issues (es: undefined reference to ...) risolvi aggiungendo il file .o a OBJS

-------------------------------

il common gnd tra arduino e alimentazione (serve?)

-------------------------------

RICORDA DI FARE UN CLAMP DI e_integral (non solo di u_pwm) perché non voglio che aumenti all'infinito se c'è qualche problema
altrimenti poi non si ristabilizza più

-------------------------------

Per usare mlx installare prima:
sudo apt-get update && sudo apt-get install xorg libxext-dev zlib1g-dev libbsd-dev

-------------------------------

mi sembra che le printf chiamate nei thread siano buffered, in particolare mi sembra che:
- thread_read: printf line buffered
- thread_write: printf a volte fully buffered a volte line buffered

printf line buffered : non scrive su terminale subito a meno che non metti '\n' a fine stringa
printf fully buffered : non scrive su terminale finché il buffer associato alla printf non è pieno

NOTA: si può forzare il print su terminale chiamando fflush(stdout); in qualsiasi momento

NOTA: da quello che ho capito non c'è un modo per settare la modalità di buffering della printf nei thread
--> tocca usare fflush(stdout)

-------------------------------

TROVATO IL PROBLEMA DEL SEGFAULT: i due thread accedono alla stessa porta seriale e vanno in conflitto
--> tocca gestire con dei mutex
