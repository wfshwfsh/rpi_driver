#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "test.h"

#define I2C_RD 1
#define I2C_WR 0

// Opens the specified I2C device.  Returns a non-negative file descriptor
// on success, or -1 on failure.
int open_i2c_device(const char * device)
{
	int fd = open(device, O_RDWR);
	if (fd == -1)
	{
		perror(device);
		return -1;
	}
	return fd;
}
 

int i2c_write(int fd, uint8_t address, uint8_t* buf)
{
	struct i2c_msg message[1];
#if 0
	uint8_t* val;
	val = buf;

	APP_D("val = 0x%x", *val);
	
	
	message[0].addr = address;
	message[0].flags = I2C_WR;
	message[0].len = 1;
	message[0].buf = val;
#else
	APP_D("buf = 0x%x", *buf);

	uint8_t val[2] = {
		(uint8_t)((*buf&0xf0)),
		(uint8_t)((*buf&0x0f)<<4),
	};

	APP_D("val[0] = 0x%x", val[0]);
	APP_D("val[1] = 0x%x", val[1]);


	message[0].addr = address;
	message[0].flags = I2C_WR;
	message[0].len = 2;
	message[0].buf = val;


#endif	

	struct i2c_rdwr_ioctl_data ioctl_data = { &message, 1/*num of i2c msg*/};
	
	int result = ioctl(fd, I2C_RDWR, &ioctl_data);
	if (result != 1)
	{
		perror("failed to set target");
		return -1;
	}
	return 0;
}

int main()
{
	int result = 0;
	// Choose the I2C device.
	const char * device = "/dev/i2c-1";

	// Set the I2C address of the Jrk (the device number).
	const uint8_t address = 0x27;

	int fd = open_i2c_device(device);
	if (fd < 0)
	{	APP_E("Error: can not open device: %s", device);	return -1;}

	unsigned char buf[1];

#if 0
	buf[0]=0x02;
	i2c_write(fd, 0x27, buf);
	usleep(10000);
	
	buf[0]=0x0f;
	i2c_write(fd, 0x27, buf);
	usleep(10000);

	buf[0]=0x30;
	//i2c_write(fd, 0x27, buf);
	usleep(10000);

	buf[0]=0x40;
	//i2c_write(fd, 0x27, buf);
	usleep(10000);

	buf[0]=0x80;
	//i2c_write(fd, 0x27, buf);
	usleep(10000);

	buf[0]=0x33;
	i2c_write(fd, 0x27, buf);
	usleep(10000);
#else

	//function set
	buf[0] = 0x20;
	i2c_write(fd, 0x27, buf);
	//i2c_write(fd, 0x0, buf);

	while(1)
	{
		buf[0]=0x0f;
		i2c_write(fd, 0x27, buf);

		sleep(3);

		buf[0]=0x8;
		i2c_write(fd, 0x27, buf);

		sleep(5);
	}
	//buf[0]=0x08;
	//i2c_write(fd, 0x27, buf);

#endif

	close(fd);
	return 0;
}
