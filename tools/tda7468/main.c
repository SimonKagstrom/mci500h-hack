#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/i2c-dev.h>

enum tda7468d_functions {
	FUNC_INPUT_SELECT_AND_MIC = 0,
	FUNC_INPUT_GAIN = 1,
	FUNC_SURROUND = 2,
	FUNC_VOLUME_LEFT = 3,
	FUNC_VOLUME_RIGHT = 4,
	FUNC_TREBLE_AND_BASS = 5,
	FUNC_OUTPUT = 6,
	FUNC_BASS_ALC = 7
};

union tda7468d_input_and_mic
{
	struct {
		uint8_t input    : 3;
		uint8_t mute_on  : 1;
		uint8_t mic_gain : 2;
		uint8_t mic_off  : 1;
	} bits;
	uint8_t b;
};

union tda7468d_input_gain
{
	struct {
		uint8_t gain : 3;
	} bits;
	uint8_t b;
};

union tda7468d_surround {
	struct {
		uint8_t surr_on   : 1;
		uint8_t gain      : 2; /* 0..12db, steps of 3db */
		uint8_t inverting : 3; /* 7: mute, 3: 0% */
		uint8_t buffer_gain : 1; /* 1: 0db, 0: 6db */
	} bits;
	uint8_t b;
};

union tda7468d_volume {
	struct {
		uint8_t steps_1db : 3; /* 0 .. -7db */
		uint8_t steps_8db : 3; /* 0 .. -56db, 8db steps*/
		uint8_t volume_2  : 2; /* 0 .. -24db, 8db steps*/
	} bits;
	uint8_t b;
};

union tda7468d_output {
	struct {
		uint8_t mute : 1; /* 1: mute off */
	} bits;
	uint8_t b;
};

union tda7468d_bass_alc {
	struct {
		uint8_t alc_mode : 1; /* 1: on */
		uint8_t detector : 1; /* 1: on */
		uint8_t release_current_circuit : 1; /* 1: on */
		uint8_t attack_time_resistor : 2;
		uint8_t threshold : 2;
		uint8_t attack_mode : 1;
	} bits;
	uint8_t b;
};

static void tda7468d_write(enum tda7468d_functions fd,
		uint8_t func, uint8_t val)
{
	char buf[2];
	int r;

	buf[0] = func;
	buf[1] = val;

	r = write(fd, buf, sizeof(buf));
	if (r < 0) {
		fprintf(stderr, "write error: %d\n", r);
		exit(1);
	}
}



static int open_i2c(const char *dev, int client_addr)
{
	int fd;

	fd = open(dev, O_RDWR);
	if (fd < 0)
		return fd;

	if (ioctl(fd, I2C_SLAVE, client_addr) < 0) {
		close(fd);
		return -1;
	}

	return fd;
}

int main(int argc, const char *argv[])
{
	return 0;
}
