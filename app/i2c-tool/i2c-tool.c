#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <errno.h>

#define I2C_DEV				"/dev/i2c-2"
#define SLAVE_ADDR			0x32

int main(int argc, char *argv[])
{
	int fd;
	int ret = 0;
	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data ioctl_data;
	unsigned char reg = 0xe;
	unsigned char val = 0;

	fd = open(I2C_DEV, O_RDWR);
	if (0 > fd)
	{
		printf("%s open fail\n", I2C_DEV);
		return -1;
	}
	msgs[0].addr= SLAVE_ADDR;
	msgs[0].len= 1;
	msgs[0].buf= &reg;
	msgs[1].addr= SLAVE_ADDR;
	msgs[1].flags|= I2C_M_RD;
	msgs[1].len= 1;
	msgs[1].buf= &val;
	ioctl_data.nmsgs= 1;
	ioctl_data.msgs= msgs;

	ret = ioctl(fd, I2C_RDWR, &ioctl_data);printf("ret=%d, %s\n", ret, strerror(errno));
	ioctl_data.msgs= &msgs[1];
	ret = ioctl(fd, I2C_RDWR, &ioctl_data);printf("ret=%d, val=%d\n", ret, val);

	return 0;
}

