#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>

/* From the Linux driver */
#define IOCTRL_SEND_LCDCMD          0x100
#define IOCTRL_CLEAR_LCD            0x101
#define IOCTRL_DRAW_LINE            0x102
#define IOCTRL_SEND_STRING          0x103
#define IOCTRL_SETUPDATE_AREA       0x104
#define IOCTRL_UPDATE_NOW           0x105
#define IOCTRL_GET_LCD_WIDTH        0x106
#define IOCTRL_GET_LCD_HEIGHT       0x107
#define IOCTRL_GET_LCD_BPP          0x108
#define IOCTRL_RESET_LCD            0x109
#define IOCTRL_INC_LCD_CONTRAST     0x10A
#define IOCTRL_DEC_LCD_CONTRAST     0x10B
#define IOCTRL_SET_PID_SIGNAL       0x10C
#define IOCTRL_SET_LIGHTPIN_HIGH    0x10D
#define IOCTRL_SET_LIGHTPIN_LOW     0x10E
#define IOCTRL_GET_DISPLAY_REVERSE_FLAG  0x10F
#define IOCTRL_SET_LCD_CONTRAST     0x110
#define IOCTRL_SET_LCD_RELATIVE_CONTRAST 0x111
#define IOCTRL_GET_LCD_SEND_FINISH_FLAG  0x112
#define IOCTRL_SET_PPTR_FLAG        0x113
#define IOCTRL_DISPLAY_ENABLE       0x114
#define IOCTRL_DISPLAY_DISABLE      0x115
#define IOCTRL_GET_TFT_TYPE         0x116
//below is for debug test
#define IOCTRL_SEND_LCDDATABYTE     0x20F
#define IOCTRL_DISP_DATA            0x213
#define IOCTRL_WRITE_LCDCTRL_REG    0x214
#define IOCTRL_READ_LCDCTRL_REG     0x215
#define IOCTRL_WRITE_LCDCTRL_DATA   0x216


static unsigned long screen_w, screen_h;
static unsigned long bytes_per_pixel;
static unsigned long val;
static unsigned long mem_size = 0x00039000;

static void *screen;

static void usage(void)
{
	printf("Usage: lcd-test <dev> <x> <y> <w> <h> <val-to-write>\n");

	exit(1);
}

void write_24(int w, int h, int ix, int iy)
{
	int x, y;
	uint8_t *p = screen;

	for (y = iy; y < iy + h; y++)
	{
		for (x = ix; x < ix + w; x++) {
			int offs = (y * screen_w + x) * 3;

			p[ offs + 0 ] = (val & 0xff) >> 0;
			p[ offs + 1 ] = (val & 0xff00) >> 8;
			p[ offs + 2 ] = (val & 0xff0000) >> 16;
		}
	}
}


void write_32(int w, int h, int ix, int iy)
{
	int x, y;
	uint32_t *p = screen;

	for (y = iy; y < iy + h; y++)
	{
		for (x = ix; x < ix + w; x++)
			p[ y * screen_w + x ] = (uint32_t)val;
	}
}


void write_16(int w, int h, int ix, int iy)
{
	int x, y;
	uint16_t *p = screen;

	for (y = iy; y < iy + h; y++)
	{
		for (x = ix; x < ix + w; x++)
			p[ y * screen_w + x ] = (uint16_t)val;
	}
}

void write_8(int w, int h, int ix, int iy)
{
	int x, y;
	uint8_t *p = screen;

	for (y = iy; y < iy + h; y++)
	{
		for (x = ix; x < ix + w; x++)
			p[ y * screen_w + x ] = (uint8_t)val;
	}
}

static void do_ioctl(int fd, int what, void *arg)
{
	if (ioctl(fd, what, arg) < 0) {
		perror("ioctl");
		usage();
	}
}

int main(int argc, const char *argv[])
{
	int x, y, w, h;
	int bpp;
	int fd;

	if (argc != 7)
		usage();

	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		usage();
	screen = mmap(NULL, mem_size, PROT_WRITE, MAP_SHARED, fd, 0);
	if (screen == MAP_FAILED)
	{
		printf("Can't mmap %s\n", argv[1]);
		perror("mmap");
		usage();
	}


	do_ioctl(fd, IOCTRL_GET_LCD_BPP, &bpp);

	if (bpp == 8)
		bytes_per_pixel = 1;
	else if (bpp == 16)
		bytes_per_pixel = 2;
	else if (bpp == 24)
		bytes_per_pixel = 3;
	else if (bpp == 32)
		bytes_per_pixel = 4;


	do_ioctl(fd, IOCTRL_GET_LCD_WIDTH, &screen_w);
	do_ioctl(fd, IOCTRL_GET_LCD_HEIGHT, &screen_h);

	printf("Screen is %lux%lu, %u bits per pixel\n", screen_w, screen_h, bpp);

	do_ioctl(fd, IOCTRL_DISPLAY_ENABLE, NULL);
	do_ioctl(fd, IOCTRL_CLEAR_LCD, NULL);

	x = strtoul(argv[2], NULL, 0);
	y = strtoul(argv[3], NULL, 0);
	w = strtoul(argv[4], NULL, 0);
	h = strtoul(argv[5], NULL, 0);

	val = strtoul(argv[6], NULL, 0);

	if ( y * screen_w + x * bytes_per_pixel > mem_size )
	{
		printf("Would write outside of memory\n");
		usage();
	}

	switch (bytes_per_pixel)
	{
	case 1:
		write_8(w, h, x, y);
		break;
	case 2:
		write_16(w, h, x, y);
		break;
	case 3:
		write_24(w, h, x, y);
		break;
	case 4:
		write_32(w, h, x, y);
		break;
	default:
		printf("Illegal byte-per-pixel, bpp %d\n", bpp);
		usage();
		break;
	}

	do_ioctl(fd, IOCTRL_UPDATE_NOW, NULL);

	msync(screen, mem_size, MS_SYNC);
	munmap(screen, mem_size);
	close(fd);

	return 0;
}
