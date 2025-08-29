To build and flash on MCU board:
    $> make flash

Pinout:
PD2 -> ENCA
PD3 -> ENCB
PD6 -> PWM (ENable)
PB2 -> IN1
PB3 -> IN2

Usiamo 
Timer 0 (8  bit) per la PWM
Timer 1 (16 bit) per il sampling time ogni 20ms
Timer 2 (8  bit) per loggare a schermo ogni 1000ms
                 [visto che sono solo 8bit è difficile fare un interrupt ogni 1000ms perche il counter conta solo fino a 255
                  allora nella ISR si conta da software e quando si è raggiunti i 1000 tick solo allora si attiva la flag]
                 (anche con il prescaler massimo 1024 non si riesce)
                 [SOSTITUISCI CON UN TIMER 16 bit con l'Arduino Mega!!!!!!!!!!!]

[NOTA: WGMxy è il CTC bit che fa si che una volta triggerato l'interrupt interno
       il counter si resetti a 0 senza continuare a contare fino all' overflow (CREDO)]
