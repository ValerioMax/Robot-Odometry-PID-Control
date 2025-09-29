Pin definitions and so code refers to Arduino Mega!!!!

To build and flash on MCU board:
    $> make flash

Pin connections for Arduino Mega:
[ sx e dx avendo la base del triangolo del robot davanti e la punta verso fuori ]

Encoder dx:
5V  -> 5V           [blue encoder]
GND -> GND          [green encoder]
50  -> ENCA         [yellow encoder]
52  -> ENCB         [white encoder]

HBridge Motor dx:
7 -> E1 (pwm1)
22 -> A1 (in1_1)
24 -> B1 (in1_2)

Encoder sx:
5V  -> 5V           [blue encoder]
GND -> GND          [green encoder]
51  -> ENCB         [white encoder]
53  -> ENCA         [yellow encoder]

HBridge Motor sx:
6 -> E1 (pwm1)
23 -> B1 (in1_2)
25 -> A1 (in1_1)
