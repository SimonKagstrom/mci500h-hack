#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>


/*
 * Function command codes for io_ctl.
 */
#define BUTTONS_GET_WAVE	1
#define BUTTONS_SET_WAVE	2
#define BUTTONS_GET_KEY	        3
#define BUTTONS_SET_KEY	        4

#define BUTTONS_SET_SIGNAL      5
#define BUTTONS_GET_FLAGS		6
#define BUTTONS_SET_KEYPAD_TYPE 7
#define BUTTONS_SET_RAW_KEY_MAPTABLE 8
#define BUTTONS_GET_RAW_KEY_MAPTABLE 9
#define BUTTONS_GET_KEY_MAPTBL_SIZE 10	//get maptable size
#define BUTTONS_SET_DEBUG_FLAG 11

#define BUTTONS_FLAGS_WAVE	0x01	//RC6 wave got
#define BUTTONS_FLAGS_KEY		0x02	//Key event!!


enum key_t_{
	Virtual_Key_Idle,

	/* 0x1 */
	physical_KEY_DBB,
	physical_KEY_IS,
	physical_KEY_MUTE,
	physical_key_Left_Reverse,
	physical_KEY_PLAY,
	physical_key_Right_Forward,
	physical_KEY_STOP,

	/* 0x8 */
	physical_KEY_SOURCE,
	physical_key_Down_Next,
	physical_key_Mark,
	physical_KEY_VOLUME_UP,
	physical_KEY_VOLUME_DOWN,
	physical_key_Music_broadcast,
	physical_key_Music_follows,
	physical_key_REC,


	/* 0x11 */
	physical_KEY_POWER_STANDBY,
	physical_key_Like_Artist,
	physical_key_Like_Genre,
	physical_key_Match_Genre,
	physical_key_Menu,
	physical_key_Up_Previous,
	physical_key_View,
	physical_key_Dim,

	/* 0x19 */
	virtual_VIEW_UP,
	virtual_VIEW_DOWN,
	virtual_PLAY_UP,
	virtual_PLAY_DOWN,
	virtual_VIEW_SOURCE,	// add this def under Vincent's demand
	virtual_DBB_SOURCE,

	/* 0x20 */
	physical_key_EJECT,
	physical_key_ERROR_ADVALUE	//add by alex
};


static void usage(void)
{
	printf("Usage: test-buttons <dev>\n");
	exit(1);
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
	int fd;

	if (argc < 2)
		usage();

	fd = open(argv[1], O_RDONLY);
	if (fd < 0)
		usage();

	while (1) {
		unsigned int data;
		key_t key;

		do_ioctl(fd, BUTTONS_GET_KEY, &data);
		key = (key_t)(data & 0xff);

		if (key != Virtual_Key_Idle)
			printf("Key: %x\n", key);

		usleep(200);
	}

	return 0;
}
