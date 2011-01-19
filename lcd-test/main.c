#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

static unsigned long screen_w, screen_h;
static unsigned long bytes_per_pixel;
static unsigned long val;
static unsigned long mem_size = 0x00039000;

static void *screen;

static void usage(void)
{
	printf("Usage: lcd-test <dev> <screen-width> <screen-height> <x> <y> <w> <h> <bytes-per-pixel> <val-to-write>\n");

	exit(1);
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


int main(int argc, const char *argv[])
{
	int x, y, w, h;
	int fd;

	if (argc != 10)
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

	screen_w = strtoul(argv[2], NULL, 0);
	screen_h = strtoul(argv[3], NULL, 0);

	x = strtoul(argv[4], NULL, 0);
	y = strtoul(argv[5], NULL, 0);
	w = strtoul(argv[6], NULL, 0);
	h = strtoul(argv[7], NULL, 0);

	bytes_per_pixel = strtoul(argv[8], NULL, 0);
	val = strtoul(argv[9], NULL, 0);

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
	case 4:
		write_32(w, h, x, y);
		break;
	default:
		printf("Illegal byte-per-pixel\n");
		usage();
		break;
	}

	msync(screen, mem_size, MS_SYNC);
	munmap(screen, mem_size);
	close(fd);

	return 0;
}
