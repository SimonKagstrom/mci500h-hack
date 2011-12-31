#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define panic(fmt...) do { \
	fprintf(stderr, "%s: ", __FUNCTION__); \
	fprintf(stderr, fmt); \
	exit(1); \
} while (0)


static void usage(void);

static uint8_t i2c_read(int fd, int dev, int addr)
{
	struct i2c_msg msgs[2];
	struct i2c_rdwr_ioctl_data data;
	uint8_t wbuf;
	uint8_t buf[1];
	int r;

	wbuf = addr;
	msgs[0].addr = dev;
	msgs[0].buf = &wbuf;
	msgs[0].flags = 0;
	msgs[0].len = sizeof(wbuf);

	msgs[1].addr = dev;
	msgs[1].buf = buf;
	msgs[1].flags = I2C_M_RD;
	msgs[1].len = sizeof(buf);

	data.msgs = msgs;
	data.nmsgs = 2;

	r = ioctl(fd, I2C_RDWR, &data);
	if (r < 0)
		panic("read error: %d\n", r);

	return buf[0];
}

static void do_dump(int fd, int dev, int n_opts, const char **options)
{
	int col, row;
	uint8_t data;

	for (row = 0; row < 16; row++) {
		for (col = 0; col < 16; col++) {
			data = i2c_read(fd, dev, row * col);

			printf("%02x ", data);
		}
		printf("\n");
	}
}

static int open_i2c(const char *dev, int client_addr)
{
	int fd;

	fd = open(dev, O_RDWR);
	if (fd < 0)
		return fd;

	return fd;
}

static void close_i2c(int fd)
{
	close(fd);
}

static void usage(void)
{
	printf("Usage: i2c-read dev <command> [options]\n\n"
			"   dump: Dump all registers\n"
			"   write <reg> <value>: Write to some register\n"
			"   ... More to come\n"
			);
	exit(1);
}

int main(int argc, const char *argv[])
{
	const char *dev;
	const char *command;
	const char **options;
	int n_opts;
	int fd;

	if (argc < 3)
		usage();
	dev = argv[1];
	command = argv[2];
	options = &argv[3];
	n_opts = argc - 2;

	fd = open_i2c("/dev/i2c-1", atoi(dev));
	if (fd < 0)
		panic("Can't open I2C device\n");

	if (strcmp(command, "dump") == 0)
		do_dump(fd, atoi(dev), n_opts, options);

	close_i2c(fd);

	return 0;
}
