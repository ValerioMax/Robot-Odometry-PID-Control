NAME = plotting
CC = gcc
BUILD_DIR = build

# libraries
MLX_FLAGS = -Llib/. -lmlx_Linux -lX11 -lXext -lm #-lz -lXrender
MLX_LIB = lib/libmlx_Linux.a

# includes
INCLUDE_DIR = include

INCLUDE_THREAD_READ_DIR = $(INCLUDE_DIR)/thread_read
INCLUDE_THREAD_WRITE_DIR = $(INCLUDE_DIR)/thread_write
INCLUDE_GRAPHICS = $(INCLUDE_DIR)/graphics

INCLUDES = -I $(INCLUDE_THREAD_READ_DIR) -I $(INCLUDE_THREAD_WRITE_DIR) -I $(INCLUDE_GRAPHICS)

# sources
SRC_DIR = src

THREAD_READ_SRCS_DIR =	$(SRC_DIR)/thread_read
THREAD_WRITE_SRCS_DIR =	$(SRC_DIR)/thread_write
GRAPHICS_SRCS_DIR = $(SRC_DIR)/graphics


SRCS = $(wildcard $(THREAD_READ_SRCS_DIR)/*.c) \
       $(wildcard $(THREAD_WRITE_SRCS_DIR)/*.c) \
       $(wildcard $(GRAPHICS_SRCS_DIR)/*.c) \
       $(SRC_DIR)/main.c

# NOTA: $(wildcard $(THREAD_READ_SRCS_DIR)/*.c) è giusto mentre $(THREAD_READ_SRCS_DIR)/*.c è sbagliato:
#		Nel secondo caso make interpreta "*" come un carattere e non come un pattern!
#		SRCS verrebbe assegnata a "src/thread_read/*.c src/thread_write/*.c ..."
#
#		Quando make chiamerà patsubst su SRCS farà matchare il pattern $(SRC_DIR)/%.c con le stringhe in SRCS
#		ottenendo wildcard % letteralmente come "thread_read/*" "thread_write/*" ...
#		e sostuendole nel pattern $(SRC_DIR)/%.c ottenendo letteralmente "build/objs/thread_read/*.o" "build/objs/thread_write/*.o ..."
#
#		Questi nomi di file NON hanno senso per il compilatore che si aspetta VALIDI FILE PATH e non PATTERN!

# objects
OBJ_DIR = $(BUILD_DIR)/obj
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# NOTA: funzione patsubst:
#		Considera tutti i file nella dir SRCS,
#		Mette nella wildcard % il percorso di tutti i file .c alla radice di tutte le dir annidate a partire da SRC_DIR
#			( es: "src/ciao.c			--> % = "ciao" )
#		 	( es: "src/thread1/file.c"	--> % = "thread1/file" )
#		Per ognunno sostituisce il relativo % nel pattern "$(OBJ_DIR)/%.o"
#		es: "src/thread1/file.c" --> "build/obj/thread1/file.o"

all : $(NAME)

# links objects into executable into build dir
$(NAME): $(OBJS)
	$(CC) $(OBJS) -o $(BUILD_DIR)/$(NAME) $(MLX_FLAGS)

# compiles sources into objects
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) -c $< -o $@ $(INCLUDES)

clean :
	rm $(NAME)
	rm -rf $(BUILD_DIR)


# NOTA: Flusso di esecuzione:
#		- chiamare da terminale "make" senza nulla fa chiamare la regola all
#		- regola all deve risolvere la dipendenza $(NAME), quindi chiama regola $(NAME)
#		- regola $(NAME) deve risolvere la dipendenza $(OBJS)
#		  $(OBJS) si espande come una lista di file .o "build/obj/thread/f1.o build/obj/thread/f2.o build/obj/thread/f3.o"
#		  per ognuno di questi viene chiamata la regola $(OBJ_DIR)/%.o
#		  ( es: per build/obj/thread/f1.o la regola che viene chiamata è build/obj/thread/f1.o )
#		- regola $(OBJ_DIR)/%.o deve risolvere la dipendenza $(SRC_DIR)/%.c
#		  che si espande semplicemente come "src/%.c" (% viene dal pattern del file .o (es: "thread/f1"))
#		- questa regola ha soddisfatto tutte le dipendenze e quindi vengono eseguite le sue istruzioni relative
#		- quando termina a cascata anche la regola che l'aveva chiamata ha soddisfatto le dipendenze e
#		  procede a eseguire le sue istruzioni
#
#		- $@ espande come il nome della regola
#		- $< espande come il nome del primo prerequisito 