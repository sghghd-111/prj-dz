#include "serial_port.h"
//#include "plat_log.h"
//#include "log.h"
//#include "ql_uart.h"
#if 0
int serial_open(char* dev_path, int baudrate, int data_bit, int stop_bit, char judge_mode)
{
	int ret = 0;
	int fd = -1;
	ST_UARTDCB dcb = {
		.flowctrl = FC_NONE,	//none flow control
		.databit = DB_CS8,	//databit: 8
		.stopbit = SB_1,	//stopbit: 1
		.parity = PB_NONE,	//parity check: none
		.baudrate = B_115200	//baudrate: 115200
	};

	fd = Ql_UART_Open("/dev/ttyHS0", 115200, FC_NONE);
	printf("%s b=%d, d=%d, s=%d, judgemode %c.\n", dev_path, baudrate, data_bit, stop_bit, judge_mode);
	if (0 >= fd)
	{
		printf("open %s error, erron=%d.\n", dev_path, errno);
		goto SERIAL_OPEN_ERR;
	}

	ret = Ql_UART_SetDCB(fd, &dcb);
	if (ret)
	{
		printf("set serial opt %s error, erron=%d.\n", dev_path, errno);
		goto SERIAL_SET_OPT_ERR;
	}
	return fd;
SERIAL_SET_OPT_ERR:
	close(fd);
SERIAL_OPEN_ERR:
	return fd;
}

void serial_close(int fd)
{
	if (0 < fd)
	{
//		tcsetattr (fd, TCSADRAIN, &opt_old);
		close(fd);
	}
}

void serial_data_print(char* head, char* buf, int len)
{
    int i = 0;
    printf("%s:", head);
    for (i=0; i<len; i++)
    {
		printf("%.2x ", buf[i]);
    }
    printf("\n");
}

int serial_read(int fd, char* buff, int len)
{
    int ret = 0;
    struct timeval tv;
    fd_set rd_fs;

    FD_ZERO(&rd_fs);
    FD_SET(fd, &rd_fs);
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    ret = select(fd+1, &rd_fs, NULL, NULL, &tv);

    if (0 > ret)
    {
        return -1;
    }
    else if (0 == ret)
    {
        return 0;
    }
    else
    {
        if (FD_ISSET(fd, &rd_fs))
        {
            ret = Ql_UART_Read(fd, buff, len);
//		    if (0 < ret)
		    {
//		    	buff[ret] = 0;
//				log_wr("r", "[%d]%s", ret, buff);
			}
//			printf("rx:%s", buff);
//			serial_data_print("recv", buff, ret);
        }
    }

    return ret;
}

int serial_write(int fd, char* buff, int len)
{
	int ret = 0;
	ret = Ql_UART_Write(fd, buff, len);
//	if (ret)
//		printf("tx:%s", buff);
//		serial_data_print("send", buff, ret);
	return ret;
}
#else

struct termios opt_old;

int serial_set_opt(int fd, int baudrate, int data_bit, int stop_bit, char judge_mode)
{
	int ret = -1;
    struct termios opt;

	memset(&opt, 0, sizeof(struct termios));
	ret = tcgetattr(fd, &opt_old);
    if  (ret) 
    { 
//        perror("get attr error, ret=%d.\n", ret);
        return ret;
    }
	cfmakeraw (&opt);

	//boadrate
	printf("baudrate=%d.\n", baudrate);
	switch (baudrate)
	{
	case 2400:
//		opt.c_cflag = B2400;
		cfsetispeed(&opt, B2400);
		cfsetospeed(&opt, B2400);
		break;
	case 4800:
		cfsetispeed(&opt, B4800);
		cfsetospeed(&opt, B4800);
		break;
	case 9600:
		cfsetispeed(&opt, B9600);
		cfsetospeed(&opt, B9600);
		break;
	case 115200:
		opt.c_cflag = B115200;
//		cfsetispeed(&opt, B115200);
//		cfsetospeed(&opt, B115200);
		break;
	case 230400:
		opt.c_cflag = B230400;
		break;
	case 4000000:
		opt.c_cflag = B4000000;
		cfsetispeed(&opt, B4000000);
		cfsetospeed(&opt, B4000000);
printf("wooooooooooo\n");
		break;
	default:
		cfsetispeed(&opt, B2400);
		cfsetospeed(&opt, B2400);
		break;
	}
	opt.c_cflag |= CLOCAL | CREAD;
	opt.c_cflag &= ~CSIZE;
	printf("judge mode=%c.\n", judge_mode);
    switch(judge_mode)
    {
    case 'o':
    case 'O':                     //\u5947\u6821\u9a8c
        opt.c_cflag |= PARENB;
        opt.c_cflag |= PARODD;
        opt.c_iflag |= (INPCK);
		opt.c_iflag &= ~(ISTRIP);
        break;
		
	case 'e':
    case 'E':                     //\u5076\u6821\u9a8c
        opt.c_iflag |= (INPCK);//|ISTRIP);
        opt.c_cflag |= PARENB;
        opt.c_cflag &= ~PARODD;
//		opt.c_iflag &= ~(ISTRIP);
        break;

	case 'n':
    case 'N':                    //\u65e0\u6821\u9a8c
        opt.c_cflag &= ~PARENB;
        opt.c_iflag &= ~(INPCK);
        break;
	default:
		{
			printf("input judge mode is %c err ret.\n", judge_mode);
			return -1;
		}
		break;
    }

//	opt.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
//	opt.c_iflag &= ~(BRKINT | ICRNL /*| INPCK*/ | ISTRIP | IXON);
	//e judge mode

	//stop bit 1
	printf("stop_bit=%d.\n", stop_bit);
	switch (stop_bit)
	{
	case 1:
		opt.c_cflag &= ~CSTOPB;
		break;
	case 2:
		opt.c_cflag |= CSTOPB;
		break;
	default:
		opt.c_cflag &= ~CSTOPB;
		break;
	}
	//data bit 8
	opt.c_cflag &= ~(CSIZE);
	switch (data_bit)
	{
	case 7:
		opt.c_cflag |= CS7;
		break;
	case 8:
		opt.c_cflag |= CS8;
		break;
	default:
		opt.c_cflag |= CS8;
		break;
	}

	opt.c_cflag	  |= (CLOCAL | CREAD);
	opt.c_lflag  &= ~(ICANON | ECHO | ECHOE | ISIG); 
	opt.c_oflag  &= ~OPOST;  
	opt.c_iflag &= ~(IXON | IXOFF | IXANY);	
    opt.c_oflag &= ~OPOST;
	opt.c_cc[VTIME] = 1; // unit: 100ms
	opt.c_cc[VMIN] = 255;//COMMUNIT characters

	tcflush (fd, TCIFLUSH);
	if (tcsetattr(fd, TCSANOW, &opt)<0)
	{
		printf("set opt error ret\n");
		return -1;
	}
    printf("set done!\n");
    return 0;
}

int serial_open(char* dev_path, int baudrate, int data_bit, int stop_bit, char judge_mode)
{
	int ret = 0;
	int fd = -1;

	fd = open(dev_path, O_RDWR|O_NOCTTY);
	printf("%s b=%d, d=%d, s=%d, judgemode %c.\n", dev_path, baudrate, data_bit, stop_bit, judge_mode);
	if (0 >= fd)
	{
		printf("open %s error, erron=%d.\n", dev_path, errno);
		goto SERIAL_OPEN_ERR;
	}

	ret = serial_set_opt(fd, baudrate, data_bit, stop_bit, judge_mode);
	if (ret)
	{
		printf("set serial opt %s error, erron=%d.\n", dev_path, errno);
		goto SERIAL_SET_OPT_ERR;
	}
	return fd;
SERIAL_SET_OPT_ERR:
	close(fd);
SERIAL_OPEN_ERR:
	return fd;
}

void serial_close(int fd)
{
	if (0 < fd)
	{
		tcsetattr (fd, TCSADRAIN, &opt_old);
		close(fd);
	}
}

void serial_data_print(char* head, char* buf, int len)
{
    int i = 0;
    printf("%s:", head);
    for (i=0; i<len; i++)
    {
		printf("%.2x ", buf[i]);
    }
    printf("\n");
}

int serial_read(int fd, char* buff, int len)
{
    int ret = 0;
    struct timeval tv;
    fd_set rd_fs;

    FD_ZERO(&rd_fs);
    FD_SET(fd, &rd_fs);
    tv.tv_sec = 2;
    tv.tv_usec = 0;
    ret = select(fd+1, &rd_fs, NULL, NULL, &tv);

    if (0 > ret)
    {
        return -1;
    }
    else if (0 == ret)
    {
        return 0;
    }
    else
    {
        if (FD_ISSET(fd, &rd_fs))
        {
            ret = read(fd, buff, len);
			if (ret)
				serial_data_print("recv", buff, ret);
        }
    }

    return ret;
}

int serial_write(int fd, char* buff, int len)
{
	int ret = 0;
	ret = write(fd, buff, len);
	if (ret)
		serial_data_print("send", buff, ret);
	return ret;
}
#endif

int main(int argc, char *argv[])
{
	char dev[32];
	int baud = 0;
	int fd = 0;
	int ret = 0;
	char buf[64];

	if (4 > argc)
	{
		printf("argc not enough...\n");
		return -1;
	}

	sprintf(dev, "/dev/%s", argv[1]);
	baud = atoi(argv[2]);

	fd = serial_open(dev, baud, 8, 1, 'n');
	if (0 > fd)
	{
		printf("open %s fail\n", dev);
		return -2;
	}
	ret = serial_write(fd, argv[3], strlen(argv[3]));
	if (0 >= ret)
	{
		printf("dev %s send fail\n", dev);
		return -3;
	}
	ret = serial_read(fd, buf, 64);

	return 0;
}

