Pin definitions and so code refers to Arduino Mega!!!!

To build and flash on MCU board:
    $> make flash

Pinout:
Arduino Uno pinout:
https://images.prismic.io/circuito/8e3a980f0f964cc539b4cbbba2654bb660db6f52_arduino-uno-pinout-diagram.png?auto=compress,format

Encoder1:
5V  -> 5V           [blue encoder]
GND -> GND          [green encoder]
PD2 -> ENCA         [yellow encoder]
PD3 -> ENCB         [white encoder]

Encoder2:
5V  -> 5V           [blue encoder]
GND -> GND          [green encoder]
PD2 -> ENCA         [yellow encoder]
PD3 -> ENCB         [white encoder]

HBridge Motor1:
PD5 -> E1 (pwm1)
PB -> A1 (in1_1)
PB -> B1 (in1_2)

HBridge Motor2:
PD6 -> E2 (pwm2)
PB2 -> A2 (in2_1)
PB3 -> B2 (in2_2)

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