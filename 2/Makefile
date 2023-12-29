

PRE:=$(shell ./mk-headers.sh)
PRE:=$(shell ./mk-shell.sh)
SRC:=$(shell find -type f -name "*.c" -or -name "*.cpp" -or -name "*.asm")
HDR:=$(shell find -type f -name "*.h" -or -name "*.hpp")
RES:=$(shell find -type f -name "*.png")
DIR:=-I ../dyn/.. $(shell find -L -mindepth 1 -type d -not -wholename "*/.*" -printf " -I %P")



CC=c++
CXX=${CC}
CPPFLAGS=$(CFLAGS)

CFLAGS+= -Werror $(DIR)

## math
#LIBS  += -lm

# libs
CFLAGS+= `pkg-config --cflags sdl2,fftw3f`
LIBS  += `pkg-config --libs   sdl2,fftw3f`

# optim
CFLAGS+= -O3

## debug sym
#CFLAGS+= -g

## parallel for
#CFLAGS+= -fopenmp
#LIBS  += -fopenmp




.PHONY : all
all : main



.PHONY : run
run : main
	./$^




OBS+=main.o
OBS+=fft.o



$(OBS) : Makefile $(SRC) $(HDR)


main : $(OBS)
	$(CC) $(CPPFLAGS) -o $@ $(OBS) $(LIBS)


#DYN+=draw_scene_gl.h
#draw_scene_gl.h : scenes/scene.obj obj2c.sh
#	./obj2c.sh scenes/scene.obj > $@


.PHONY : clean cl
clean cl :
	file * | awk '/ELF/ { gsub(/:.*/,"") ; print }' | xargs -r rm
	rm -fR deps.inc dyn



.PHONY : rebuild re
rebuild re : clean all

