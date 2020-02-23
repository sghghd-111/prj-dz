#ifndef _SERIAL_PORT_H_
#define _SERIAL_PORT_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <string.h>
//#include "osl.h"

#if 0
#ifndef LOG_NEW
#define SAFE_DEBUG(fmt, ...)	    itl_log(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define SAFE_ERR(fmt, ...)	    itl_log(LOG_ERR, fmt, ##__VA_ARGS__)
#define SAFE_INFO(fmt, ...)	    itl_log(LOG_INFO, fmt, ##__VA_ARGS__)
//#else
#define SAFE_DEBUG(fmt, ...)	    LOG_E(0, LOG_DEBUG, fmt, ##__VA_ARGS__)
#define SAFE_ERR(fmt, ...)	    LOG_E(0, LOG_ERR, fmt, ##__VA_ARGS__)
#define SAFE_INFO(fmt, ...)	    LOG_I(0, LOG_INFO, fmt, ##__VA_ARGS__)
#endif
#endif
//serial port path
//high speed serial port
#define HIGH_SERIAL_PORT1_PATH			"/dev/ttyTX0"
#define HIGH_SERIAL_PORT2_PATH			"/dev/ttyTX1"
#define HIGH_SERIAL_PORT3_PATH			"/dev/ttyTX2"
//com1
#define SERIAL_PORT1_PATH			"/dev/ttyS0"
//com2
#define SERIAL_PORT2_PATH			"/dev/ttyS1"
//com3
#define SERIAL_PORT3_PATH			"/dev/ttyS2"
#define SERIAL_PORT4_PATH			"/dev/ttyS3"


//data bit
#define SERIAL_DATA_BIT_8			8
#define SERIAL_DATA_BIT_7			7

//STOP BIT
#define SERIAL_STOP_BIT_1			0
#define SERIAL_STOP_BIT_2			1

//judge mode
#define SERIAL_JUDGE_MODE_O			'o'						//<odd judge
#define SERIAL_JUDGE_MODE_E			'e'						//<even judge
#define SERIAL_JUDGE_MODE_N			'n'						//<no judge
#define SERIAL_JUDGE_MODE_S			's'						//<space judge

extern int serial_open(char* dev_path, int baudrate, int data_bit, int stop_bit, char judge_mode);
extern int serial_write(int fd, char* buff, int len);
extern int serial_read(int fd, char* buff, int len);
extern void serial_close(int fd);
extern void serial_data_print(char* head, char* buf, int len);
#endif

