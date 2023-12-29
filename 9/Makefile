


PRE:=$(shell ./mk-headers.sh)
PRE:=$(shell ./mk-shell.sh)
SRC:=$(shell find -type f -name "*.c" -or -name "*.cpp" -or -name "*.asm")
HDR:=$(shell find -type f -name "*.h" -or -name "*.hpp")
RES:=$(shell find -type f -name "*.png")
DIR:=$(shell echo "-I ../dyn/.." && find -L -mindepth 1 -type d -not -wholename "*/.*" -printf " -I %P")





CC=c++
CXX=${CC}
CPPFLAGS=$(CFLAGS)

CFLAGS+= -Werror $(DIR)

## math
#LIBS  += -lm

# SDL
CFLAGS+= `pkg-config --cflags sdl2`
LIBS  += `pkg-config --libs   sdl2`

# ttf
CFLAGS+= `pkg-config --cflags SDL2_ttf`
LIBS  += `pkg-config --libs   SDL2_ttf`

# optim
CFLAGS+= -O3

## debug sym
#CFLAGS+= -g

## parallel for
#CFLAGS+= -fopenmp
#LIBS  += -fopenmp



.PHONY: all
all: main


.PHONY: run
run: main 
	./main





OBS+=main.o
OBS+=frame.o
OBS+=loop.o
OBS+=kmeans.o
OBS+=audio.o
OBS+=RING.o
OBS+=detect.o
OBS+=DELAY.o
OBS+=TTF_printf.o




$(OBS): Makefile $(SRC) $(HDR)


main: $(OBS)
	$(CC) $(CFLAGS) -o $@ $(OBS) $(LIBS)
#	strip $@


#DYN+=draw_scene_gl.h
#draw_scene_gl.h: scenes/scene.obj obj2c.sh
#	./obj2c.sh scenes/scene.obj > $@


.PHONY: clean cl
clean cl:
	file * | awk '/ELF/ { gsub(/:.*/,"") ; print }' | xargs -r rm
	rm -fR dyn


.PHONY: rebuild re
rebuild re: clean all


