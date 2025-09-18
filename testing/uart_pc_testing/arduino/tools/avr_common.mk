INCLUDE_DIRS=-I. -I../tools -I./include
CXX=avr-g++
CC=avr-gcc
AS=avr-gcc
AVRDUDE=avrdude


CC_OPTS_GLOBAL=\
-O3\
-funsigned-char\
-funsigned-bitfields\
-fshort-enums\
-Wall\
$(INCLUDE_DIRS)\
-DF_CPU=16000000UL\

TARGET=mega
#TARGET=uno
AVRDUDE_PORT=/dev/ttyACM0

ifeq ($(TARGET), mega)
	CC_OPTS_GLOBAL += -mmcu=atmega2560 -D__AVR_3_BYTE_PC__
	AVRDUDE_FLAGS  += -p m2560
	AVRDUDE_BAUDRATE = 115200
	AVRDUDE_BOOTLOADER = wiring
endif

ifeq ($(TARGET), uno)
	CC_OPTS_GLOBAL += -mmcu=atmega328p 
	AVRDUDE_FLAGS  += -p m328p
	AVRDUDE_BAUDRATE = 115200
    AVRDUDE_BOOTLOADER = arduino
endif

CC_OPTS=$(CC_OPTS_GLOBAL) --std=gnu99 
CXX_OPTS=$(CC_OPTS_GLOBAL) --std=c++17 
AS_OPTS=-x assembler-with-cpp $(CC_OPTS)

AVRDUDE_WRITE_FLASH = -U flash:w:$(TARGET):i
AVRDUDE_FLAGS += -P $(AVRDUDE_PORT) -b $(AVRDUDE_BAUDRATE)
AVRDUDE_FLAGS += -D -q -V -C /usr/share/arduino/hardware/tools/avr/../avrdude.conf
AVRDUDE_FLAGS += -c $(AVRDUDE_BOOTLOADER)


.phony:	clean all

all:	$(BINS) 

#common objects
$(BUILD_DIR)/%.o:	$(SRC_DIR)/%.c 
	$(CC) $(CC_OPTS) -c  -o $@ $<

$(BUILD_DIR)/%.o:	$(SRC_DIR)/%.s 
	$(AS) $(AS_OPTS) -c  -o $@ $<

$(BUILD_DIR)/%.elf:	$(BUILD_DIR)/%.o $(OBJS)
	$(CC) $(CC_OPTS) -o $@ $< $(OBJS) $(LIBS)

$(BUILD_DIR)/%.hex:	$(BUILD_DIR)/%.elf
	avr-objcopy -O ihex -R .eeprom $< $@
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$@:i #$(AVRDUDE_WRITE_EEPROM)

flash: $(BUILD_DIR)/$(BIN_NAME).hex

clean:	
	rm -rf $(OBJS) $(BINS) $(BUILD_DIR)/*.hex $(BUILD_DIR)/*~ $(BUILD_DIR)/*.o

.SECONDARY:	$(OBJS)
