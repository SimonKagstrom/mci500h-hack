#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#define  IOCTRL_SET_PID_SIGNAL         0x100
#define  IOCTRL_GET_RXBUF_RECEIV_LEN   0x101
#define  IOCTRL_GET_TXBUF_FREE_LEN     0x102
#define  IOCTRL_CLEAR_RXBUF            0x103
#define  IOCTRL_ENABLE_RECEIVE         0x104
#define  IOCTRL_DISABLE_RECEIVE        0x105
#define  IOCTRL_SET_SPEED_19200        0x106
#define  IOCTRL_SET_SPEED_7000         0x107
#define  IOCTRL_SET_RX_SWITCHPIN_LOW   0x108
#define  IOCTRL_SET_RX_SWITCHPIN_HIGH  0x109
#define  IOCTRL_SET_TX_FREQ            0x10A

struct set_pid_signal_struct
{
	int pid;
	int sig;
};


static void usage(void)
{
	printf("Usage: test-remote <dev>\n");
	exit(1);
}

static void do_ioctl(int fd, int what, void *arg)
{
	if (ioctl(fd, what, arg) < 0) {
		perror("ioctl");
		usage();
	}
}



static int g_fd;
static void handler(int signum)
{
	printf("Got sig %d!\n", signum);
}

int main(int argc, const char *argv[])
{
	struct set_pid_signal_struct ps;
	size_t len;
	int fd;

	if (argc < 2)
		usage();

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		usage();

	signal(SIGUSR1, handler);
	ps.pid = getpid();
	ps.sig = SIGUSR1;
	g_fd = fd;

	len = 10;
	printf("Setting pid/sig to %d:%d\n", ps.pid, ps.sig);
	do_ioctl(fd, IOCTRL_SET_PID_SIGNAL, &ps);
	do_ioctl(fd, IOCTRL_ENABLE_RECEIVE, NULL);

	do_ioctl(fd, IOCTRL_CLEAR_RXBUF, NULL);

	while (1) {
		char buf[128];
		int v;

		do_ioctl(fd, IOCTRL_SET_SPEED_7000, NULL);
		do_ioctl(fd, IOCTRL_SET_RX_SWITCHPIN_LOW, NULL);
		do_ioctl(fd, IOCTRL_GET_RXBUF_RECEIV_LEN, &len);
		if (len != 0)
			printf("RX buf size: %lx\n", (long)len);

		v = read(fd, &buf, len);
		if (v > 0) {
			printf("read %x\n", buf[0]);
		}
		usleep(200);
	}

	return 0;
}
