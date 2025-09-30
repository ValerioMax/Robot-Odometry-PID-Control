# Robot-Odometry-PID-Control
## Abstract
Progetto del corso di Complementi di Sistemi Operativi. \
L'obiettivo è l'implentazione di un'odometria e il controllo a distanza di un robot a tre ruote (due motrici e una libera) mediante programmazione bare-metal AVR in linguaggio C.

## Structure
Viene usato C con uno stile Object Oriented. \
Per ogni motore è presente un controllo di posizione e di velocità attraverso controllori PID. \
Per il controllo generale del robot è presente un anello chiuso semplice. \
Per la comunicazione a distanza si usa Bluetooth.

### Hardware
- 2 motori DC da 0-12V con encoder a sensore Hall.
- ponte-H per 2 motori
- Arduino Mega
- Esp32


### Software
Il codice è strutturato in due sezioni:
- **robot server:** \
    A sua volta è diviso in:

    - **mcu_arduino:** \
    Contiene la totalità della logica di controllo del robot. \
    Nel loop di controllo vengono ricevuti, interpretati e attuati i comandi dalla linea seriale, viene computata l'odometria del robot.

    - **mcu_esp32:** \
    Semplice redirect server. \
    Si occupa solamente di ricevere i comandi dall'esterno tramite bluetooth e di reindirizzarli all'Arduino tramite la UART e viceversa per il log dei dati. \
    \
    *(per la comunicazione wireless non avevo a disposizione un modulo bluetooth apposito o altro)*

- **pc client:** \
    Client multithread (scritto in C) usato per interfacciarsi con il server tramite comunicazione seriale attraverso delle termios. \
    Diviso in:

    - **thread_read:** \
    Riceve informazioni di logging, le mette a schermo e/o le plotta in tempo reale attraverso l'utilizzo della libreria grafica mlx. \
    La finestra è interattiva e si può fare lo zoom.

    - **thread_write:** \
    Si mette in ascolto dei comandi inviati nel terminale dall'utente e li inoltra all'Arduino.

## Command Interface
E' possibile controllare il robot in due modalità:
- usando comandi
- con un controllo diretto WASD

### Commands
- `attach <motor1> <motor2>` \
Abilita/Disabilita motore 1 e 2.
    - motor-i: {0: disabled, 1: enabled}

- `pwm <dir_motor1_left> <duty_cycle%_motor_left> <dir_motor_right> <duty_cycle%_motor_right>` \
Applica una velocità proporzionale al duty-cycle nella direzione specificata da dir per ognuno dei motori.
    - duty_cycle: [0, 65535]
    - dir: {-1: CCW, 0: still, 1: CW}

- `gain <kp> <ki> <kd>` \
Imposta i valori per i parametri kp, ki, kd per il controllo PID.

- `pos <pos_motor_1> <pos_motor_2>` \
Imposta la posizione target, in tick dell'encoder, per motore 1 e 2. \
Tale posizione verrà subito attuata abilitando il controllo PID di posizione per i motori.

- `rpm <rpm_motor_1> <rpm_motor_2>` \
Imposta la velocità target, in rpm, per motore 1 e 2. \
Tale posizione verrà subito attuata abilitando il controllo PID di velocità per i motori.

- `setwasd <1/0>` \
Abilita/Disabilita controllo WASD del robot.

- `goto <x> <y>` \
Imposta la posizione target in coordinate cartesiane nel sistema fixed per il robot. \
Tale posizione verrà subito attuata abilitando il controllo autonomo del robot.

### WASD
Se il controllo WASD è abilitato, il client fa entrare il terminale associato al processo in modalità non-canonical. \
Basterà spingere i tasti 'w', 'a', 's' o 'd' per controllare il robot. \
Per uscire spingere 'q'.

## NOTE
- Per il controllo di posizione settare: \
    kp = 300 \
    ki = 0 \
    kd = 15 

- per il controllo di velocità settare: \
    kp = 2500 \
    ki = 2000 \
    kd = 0

## Software Requirements
Per usare avr-gcc: 
```bash
sudo apt-get install arduino arduino-mk
```
Per usare mlx:
```bash
sudo apt-get update && sudo apt-get install xorg libxext-dev zlib1g-dev libbsd-dev
```

## Instructions
Collegati alla linea seriale 0 dell'Arduino Mega. \
Assicurati di aver scollegato i pin dell'Esp32 collegati a TX0 e RX0. \
Vai in `/mcu_arduino/`, compila e linka il binario e caricalo nella Flash dell'Arduino:
```bash
make flash
```

Una volta fatto, scollegati dalla linea e ricollega i pin all'Esp32. \
\
Vai in `/pc_client/` compila e linka l'eseguibile:
```bash
make
```

Lancia il programma:
```bash
./build/pc_client
```

Alternativamente usa un monitor seriale per connetterti alla tty `/dev/rfcomm0`.