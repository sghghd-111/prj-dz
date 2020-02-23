###########################################
#Makefile for simple programs
###########################################
INC = -I ../../include/ -I ../../libs/common/ -I ../../libs/msg/ -I ../../libs/cmdline/
LIB = -lpthread -L ../../libs/common/ -lcomm -L ../../libs/msg/ -lmsg #-L ../../libs/cmdline -lcmd

CC = arm-linux-gnueabihf-gcc
CC_FLAG = -Wall

PRG = test_app
OBJ = app_test.o

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
