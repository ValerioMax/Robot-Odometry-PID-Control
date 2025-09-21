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
UPDATE: IN REALTà NON CREDO, perchè la comunicazione serial è FULL DUPLEX quindi non è un problema
    il segfault è sparito quando ho commentato le printf senza '\n' in robot.c:
        sembra che se arduino fa printf senza '\n' da fastidio al pc client

-------------------------------

Risolvi il fatto che dopo un pò inizia a laggare il pc client

-------------------------------

Empiricamente ho trovato che per una rotazione di shaft esterno ogni motore fa 2100/2200 tick per rivoluzione completa

Attaccando DIRETTAMENTE la tensione al morsetti dei motori ho notato che gli rpm_max sono:
- Attaccando 12V --> ~88rpm
- Attaccando 10.4V --> ~77rpm
- Attaccando 8V --> ~58rpm
- Attaccando 3V --> ~20rpm

NOTA: attaccando INDIRETTAMENTE la tensione, ovvero dandogliela all'HBridge, ci sarà un calo di tensione dovuto
      alla coppia di mosfet ON dove passa la corrente.
      (Es: applicando 12V all'HBridge, con duty cycle max (100%), ottengo tensione diretta sul motore di 10.4V)

      Quindi applicando 12V all'Hbridge, gli rpm_max a cui potranno andare i motori è ~77rpm

-------------------------------

Dopo un pò di tuning ho trovato PID params:
- per controllo posizione (BUONI):
    kp = 300
    ki = 0
    kd = 15
  danno una stedy state costante di un paio di step (molto piccola rispetto agli step che fa, es: pos=10002 target=10000)
  e zero oscillazioni nell'arrivare al target

NOTA: essendo costante, questo errore si nota di più quando il comando è piccolo, quindi non farti ingannare dal grafico plottato
      se il range sulle y è piccolo!

NOTA: Ho problemi a usare il termine intergrale: o il clamping è sbagliato oppure non so bene come tunarlo.
      Qualsiasi valore che metto in ki ottengo dei risultati peggiori (maggiore steady state e errore) che senza

- per il controllo di velocità (NON BUONISSIMI):
    kp = 6000
    ki = 100
    kd = 200

-------------------------------

NOTA: è fondamentale che u e u_pwm siano di un tipo (es: float o int32_t) che possa contenere un numero molto grande.
Perché se il target è molto grande e u diventa molto grande allora potrebbe andare in overflow
facendo ottenere paradossalmente un segnale più piccolo!!!!!!
--> Si potrebbe fare un check sull'errore e su e_dt oltre che su e_integral, ma bisogna tenere conto anche di kp, ki, kd

-------------------------------

TODO: Dopo un pò il pc client che plotta inizia a laggare: risolvilo

-------------------------------

NOTA: Se usi CuteCom o altro assicurati di non essere nella mod CR/LF, perché invierebbe due new line!!
      e questo programma è scritto per riceverne solo uno per stringa: risulterebbe in due stringhe ricevute di cui una vuota ("...\n\0" e "\n\0")

-------------------------------

Al livello hardware siccome i due motori sono specchiati ho invertito i pin del motore a sx (avedo davanti la base del triangolo)
Sono scambiati sia i due pin dell'encoder che del hbridge

