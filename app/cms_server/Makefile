###########################################
#Makefile for simple programs
###########################################
INC = -I ../../include/ -I ../../libs/common/ -I ../../libs/msg/ -I ../../libs/cmdline/
LIB = -lpthread -L ../../libs/common/ -lcomm -L ../../libs/msg/ -lmsg #-L ../../libs/cmdline lcmd

CC = arm-linux-gnueabihf-gcc
CC_FLAG = -Wall -O0

PRG = cms-svr
SRC = $(wildcard *.c)
OBJ = $(patsubst %c, %o, $(SRC))
#OBJ = cms_svr.o pulse.o wtd_func.o udp_func.o bc_func.o gpio_key_wtd.o

$(PRG):$(OBJ)
	$(CC) $(OBJ) $(INC) $(LIB) -o $@

.SUFFIXES: .c .o
.c.o:
	$(CC) $(CC_FLAG) $(INC) -c $*.c -o $*.o 

.PRONY:clean
clean:
	echo "Removing linked and compiled files......"
	rm -f $(OBJ) $(PRG)

install:
	cp $(PRG) ../../install_dir/bin/
	
