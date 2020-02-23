###########################################
#Makefile for simple programs
###########################################
INC = -I ../../libs/common/ -I ../../libs/msg/
LIB = -lpthread -L ../../libs/common/ -lcomm -L ../../libs/msg/ -lmsg

CC = gcc
CC_FLAG = -Wall

PRG = cmdline
OBJ = usr_cmdline.o

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
