/*
 * $id$
 *
 * Tests the force feedback driver
 * Copyright 2001-2002 Johann Deneux <deneux@ifrance.com>
 */

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 *
 * You can contact the author by email at this address:
 * Johann Deneux <deneux@ifrance.com>
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/input.h>

#include "bitmaskros.h"


char* effect_names[] = {
	"Rumble"
};

#define N_EFFECTS 1

int main(int argc, char** argv)
{
	struct ff_effect effects[1];
	struct input_event play, stop, gain;
	int fd;
	const char * device_file_name = "/dev/input/event1";
	unsigned char ffFeatures[1 + FF_MAX/8/sizeof(unsigned char)];
	int i;

	printf("Force feedback test program.\n");
	printf("HOLD FIRMLY YOUR WHEEL OR JOYSTICK TO PREVENT DAMAGES\n\n");

	for (i=1; i<argc; ++i) {
		if (strncmp(argv[i], "--help", 64) == 0) {
			printf("Usage: %s /dev/input/eventXX\n", argv[0]);
			printf("Tests the force feedback driver\n");
			exit(1);
		}
		else {
			device_file_name = argv[i];
		}
	}

	/* Open device */
	fd = open(device_file_name, O_RDWR);
	if (fd == -1) {
		perror("Open device file");
		exit(1);
	}
	printf("Device %s opened\n", device_file_name);

	/* Set master gain to 75% if supported */
	if (testBit(FF_GAIN, ffFeatures)) {
		memset(&gain, 0, sizeof(gain));
		gain.type = EV_FF;
		gain.code = FF_GAIN;
		gain.value = 0xFFFF; /* [0, 0xFFFF]) */

		printf("Setting master gain to 100%% ... ");
		fflush(stdout);
		if (write(fd, &gain, sizeof(gain)) != sizeof(gain)) {
		  perror("Error:");
		} else {
		  printf("OK\n");
		}
	}

	memset(effects, 0, sizeof(effects));
	/* a strong rumbling effect */
	effects[0].type = FF_RUMBLE;
	effects[0].id = -1;
	effects[0].u.rumble.strong_magnitude = 0x8000;
	effects[0].u.rumble.weak_magnitude = 0;

	printf("Uploading effect rumble ... ");
	fflush(stdout);
	if (ioctl(fd, EVIOCSFF, &effects[0]) == -1) {
		perror("Error");
	} else {
		printf("OK (id %d)\n", effects[0].id);
	}

	/* Ask user what effects to play */
	do {
		printf("Enter effect number, -1 to exit\n");
		i = -1;
		if (scanf("%d", &i) == EOF) {
			printf("Read error\n");
		}
		else if (i >= 0 && i < N_EFFECTS) {
			memset(&play,0,sizeof(play));
			play.type = EV_FF;
			play.code = effects[i].id;
			play.value = 1;

			if (write(fd, (const void*) &play, sizeof(play)) == -1) {
				perror("Play effect");
				exit(1);
			}

			printf("Now Playing: %s\n", effect_names[i]);
		}
	} while (i>=0);

	/* Stop the effects */
	printf("Stopping effects\n");
	for (i=0; i<N_EFFECTS; ++i) {
		memset(&stop,0,sizeof(stop));
		stop.type = EV_FF;
		stop.code =  effects[i].id;
		stop.value = 0;
        
		if (write(fd, (const void*) &stop, sizeof(stop)) == -1) {
			perror("");
			exit(1);
		}
	}
	
	exit(0);
}
