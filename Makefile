############################################################# 
# Makefile for shared library.
#############################################################
#set your own environment option
#! /bin/bash
CC = arm-linux-gnueabihf-gcc
CC_FLAG =
 
#set your inc and lib
INC = 
LIB = -lpthread -L./

subdirs:=\
	./libs/common/\
	./libs/msg/\
	./app/cms_server/\
	./app/app_test/\
	./app/cmdline/\
	\
	\
	./modules/pulse/\
	./modules/ch438/\
	./modules/gpmcnet/\
	./modules/hx_key/\
	\

test:
	$(foreach N,$(subdirs),make -C $(N);)

#make target lib and relevant obj 
#PRG = lmsg.so
#OBJ = lib_msg.o

#all target
all:
	$(foreach dir,$(subdirs),make -C $(dir);)

.PRONY:clean
clean:
	$(foreach dir,$(subdirs),make -C $(dir) clean;)

install:
	$(foreach dir,$(subdirs),make -C $(dir) install;)

