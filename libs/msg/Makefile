############################################################# 
# Makefile for shared library.
#############################################################
#set your own environment option
CC = arm-linux-gnueabihf-gcc
CC_FLAG = -Wall -O0  -fPIC
 
#set your inc and lib
INC = -I ../common/ -I ../../include/
LIB = -lpthread -L./

#make target lib and relevant obj 
PRG = libmsg.so
OBJ = ipc_msg.o itc_msg.o

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
