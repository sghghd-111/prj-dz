############################################################# 
# Makefile for shared library.
#############################################################
#set your own environment option
CC = arm-linux-gnueabihf-gcc
CC_FLAG = -Wall -O0  -fPIC
 
#set your inc and lib
INC = 
LIB = -lpthread -L./

#make target lib and relevant obj 
PRG = libcomm.so
OBJ = plat_config.o plat_daem.o plat_log.o ms_sleep.o

#all target
all:$(PRG)

$(PRG):$(OBJ)
	$(CC) -shared -o $@ $(OBJ) $(LIB) 
.SUFFIXES: .c .o
.c.o:
	$(CC) $(CC_FLAG) $(INC) -c $*.c -o $*.o

.PRONY:clean
clean:
	echo "Removing linked and compiled files......"
	rm -f $(OBJ) $(PRG)

install:
	echo "install "
	cp $(PRG) ../../install_dir/lib/
