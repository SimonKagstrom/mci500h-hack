#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <linux/i2c-dev.h>

#define panic(fmt...) do { \
	fprintf(stderr, "%s: ", __FUNCTION__); \
	fprintf(stderr, fmt); \
	exit(1); \
} while (0)

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

static void tda7468d_write(int fd,
		enum tda7468d_functions func, uint8_t val)
{
	char buf[2];
	int r;

	buf[0] = func;
	buf[1] = val;

	r = write(fd, buf, sizeof(buf));
	if (r < 0)
		panic("write error: %d\n", r);
}

static void do_input(int fd, int n_opts, const char **options)
{
	union tda7468d_input_and_mic v;
	int i;

	v.b = 0;
	v.bits.mic_off = 1;

	for (i = 0; i < n_opts; i++) {
		const char *p = options[i];

		if (strcmp(p, "in1") == 0)
			v.bits.input = 0;
		else if (strcmp(p, "in2") == 0)
			v.bits.input = 1;
		else if (strcmp(p, "in3") == 0)
			v.bits.input = 2;
		else if (strcmp(p, "in4") == 0)
			v.bits.input = 3;

		else if (strcmp(p, "mute") == 0)
			v.bits.mute_on = 1;
		else if (strcmp(p, "unmute") == 0)
			v.bits.mute_on = 0;

		else if (strcmp(p, "mic_0") == 0) {
			v.bits.mic_off = 0;
			v.bits.mic_gain = 3;
		}
		else if (strcmp(p, "mic_6") == 0) {
			v.bits.mic_off = 0;
			v.bits.mic_gain = 2;
		}
		else if (strcmp(p, "mic_10") == 0) {
			v.bits.mic_off = 0;
			v.bits.mic_gain = 1;
		}
		else if (strcmp(p, "mic_14") == 0) {
			v.bits.mic_off = 0;
			v.bits.mic_gain = 0;
		}
	}

	tda7468d_write(fd, FUNC_INPUT_SELECT_AND_MIC, v.b);
}

static void do_gain(int fd, int n_opts, const char **options)
{
	panic("NYI\n");
}

static void do_volume(int fd, int is_left, int n_opts, const char **options)
{
}

static void do_output(int fd, int n_opts, const char **options)
{
	panic("NYI\n");
}

static void do_surround(int fd, int n_opts, const char **options)
{
	panic("NYI\n");
}

static void do_treble_bass(int fd, int n_opts, const char **options)
{
	panic("NYI\n");
}

static void do_bass_alc(int fd, int n_opts, const char **options)
{
	panic("NYI\n");
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

static void close_i2c(int fd)
{
	close(fd);
}

static void usage(void)
{
	printf("Usage: tda7468-tool <command> [options]\n\n"
			"Where <command> and [options] can be\n\n"
			""
			);
	exit(1);
}

int main(int argc, const char *argv[])
{
	const char *command;
	const char **options;
	int n_opts;
	int fd;

	if (argc < 4)
		usage();
	command = argv[1];
	options = &argv[2];
	n_opts = argc - 2;

	fd = open_i2c("/dev/i2c-1", 0x44);
	if (fd < 0)
		panic("Can't open I2C device\n");

	if (strcmp(command, "input") == 0)
		do_input(fd, n_opts, options);
	else if (strcmp(command, "gain") == 0)
		do_gain(fd, n_opts, options);
	else if (strcmp(command, "surround") == 0)
		do_surround(fd, n_opts, options);
	else if (strcmp(command, "volume-left") == 0)
		do_volume(fd, 1, n_opts, options);
	else if (strcmp(command, "volume-right") == 0)
		do_volume(fd, 0, n_opts, options);
	else if (strcmp(command, "treble") == 0)
		do_treble_bass(fd, n_opts, options);
	else if (strcmp(command, "output") == 0)
		do_output(fd, n_opts, options);
	else if (strcmp(command, "bass-alc") == 0)
		do_bass_alc(fd, n_opts, options);

	close_i2c(fd);

	return 0;
}
