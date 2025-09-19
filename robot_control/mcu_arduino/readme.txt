Pin definitions and so code refers to Arduino Mega!!!!

To build and flash on MCU board:
    $> make flash

Pin connections for Arduino Mega:

Encoder1:
5V  -> 5V           [blue encoder]
GND -> GND          [green encoder]
51  -> ENCA         [yellow encoder]
53  -> ENCB         [white encoder]

HBridge Motor1:
6 -> E1 (pwm1)
23 -> A1 (in1_1)
25 -> B1 (in1_2)
