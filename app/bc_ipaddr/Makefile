###########################################
#Makefile for simple programs
###########################################
INC = #-I ../../include/ -I ../../libs/common/ -I ../../libs/msg/ -I ../../libs/cmdline/
LIB = -lpthread #-L ../../libs/common/ -lcomm -L ../../libs/msg/ -lmsg -L ../../libs/cmdline lcmd

CC = gcc
CC_FLAG = -Wall

PRG = bc_ipaddr
OBJ = udp_func.o

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
