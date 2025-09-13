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


Mi servono classi Motor, Encoder così faccio:
X = 1, 2
encoderX.init() // in realtà la init
encoderX.read()

motorX.PID()
motorX.setmotor()

struct Motor {
    int in1_pin;
    int in2_pin;
    int pwm_pin;

    int curr_pos;
    int curr_rpm;
    int curr_dir;
    
    int err_pos;
    int err_speed;

    fun_pointer : setmotor()
    ...
}
motorX->setmotor()



