#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "test.h"

#define u8 unsigned char
#define I2C_RD 1
#define I2C_WR 0

#define SLAVE_ADDRESS 0x23

// Opens the specified I2C device.  Returns a non-negative file descriptor
// on success, or -1 on failure.
int open_device(const char * device)
{
	int fd = open(device, O_RDWR);
	if (fd == -1)
	{
		perror(device);
		return -1;
	}
	return fd;
}
 
static int talk_to_device(int fd, int addr)
{
	int ret = ioctl(fd, I2C_SLAVE, addr);
	if (ret < 0)
		printf("Failed to acquire bus access and/or talk to slave.\n");

	return ret;
}

int i2c_read(int fd, u8 *buf, int bufsize) {
	
    return read(fd, buf, bufsize);
}

int i2c_write(int fd, uint8_t address, uint8_t* buf)
{
	struct i2c_msg message[1];

	uint8_t* val;
	val = buf;

	APP_D("val = 0x%x", *val);
	
	message[0].addr = address;
	message[0].flags = I2C_WR;
	message[0].len = 1;
	message[0].buf = val;

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
	const uint8_t address = SLAVE_ADDRESS;

	int fd = open_device(device);
	if (fd < 0)
	{	APP_E("Error: can not open device: %s", device);	return -1;}

	int ret = talk_to_device(fd, address);
	if (ret < 0)
		APP_E("Failed to acquire bus access and/or talk to slave.");

	unsigned char buf[1];

	//function set
	buf[0] = 0x10;
	i2c_write(fd, address, buf);
	//i2c_write(fd, 0x0, buf);

	unsigned char data[2];
	while(1)
	{
		i2c_read(fd, data, 2);
		APP_I("data = %d %d", data[0], data[1]);
		usleep(500000);
	}

	close(fd);
	return 0;
}
