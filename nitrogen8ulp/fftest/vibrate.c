/*
 * $id$
 *
 * Tests the force feedback driver
 * Copyright 2001-2002 Johann Deneux <deneux@ifrance.com>
 * Copyright 2023 Eric Nelson <eric@nelint.com>
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
#include <linux/limits.h>
#include <dirent.h>
#include <errno.h>

#include "bitmaskros.h"


char* effect_names[] = {
	"Rumble"
};

#define N_EFFECTS 1

static void vibrate(int fd)
{
	struct ff_effect effects[1];
	struct input_event ev;

    memset(&ev, 0, sizeof(ev));
    ev.type = EV_FF;
    ev.code = FF_GAIN;
    ev.value = 0xFFFF; /* [0, 0xFFFF]) */

    fflush(stdout);
    if (write(fd, &ev, sizeof(ev)) != sizeof(ev)) {
      perror("Gain Error:");
    }

	memset(effects, 0, sizeof(effects));
	/* a strong rumbling effect */
	effects[0].type = FF_RUMBLE;
	effects[0].id = -1;
	effects[0].u.rumble.strong_magnitude = 0x8000;
	effects[0].u.rumble.weak_magnitude = 0;

	if (ioctl(fd, EVIOCSFF, &effects[0]) == -1) {
		perror("Error");
	}

    memset(&ev,0,sizeof(ev));
    ev.type = EV_FF;
    ev.code = effects[0].id;
    ev.value = 1;

    if (write(fd, (const void*) &ev, sizeof(ev)) == -1) {
        perror("Play effect");
        exit(1);
    }

    usleep(100000);

    ev.value = 0;
    if (write(fd, (const void*) &ev, sizeof(ev)) == -1) {
        perror("Play effect");
        exit(1);
    }
}

static char const inputdir[] = "/dev/input";

int main (void)
{
    int fd, rv;
    DIR *dp;
    struct dirent *ep;     
    char devpath[PATH_MAX];
    unsigned long evbits[2] = {0};

    dp = opendir (inputdir);
    if (!dp) {
        perror(inputdir);
        return -EIO;
    }

    while ((ep = readdir (dp)) != NULL) {
        snprintf(devpath, sizeof(devpath), "%s/%s", inputdir, ep->d_name);
        if (DT_DIR == ep->d_type)
            continue;

    	/* Open device */
    	fd = open(devpath, O_RDWR);
    	if (fd < 0) {
    		perror(devpath);
            continue;
        }

        rv = ioctl(fd, EVIOCGBIT(0, sizeof(evbits)), &evbits);
        if (-1 == rv) {
            perror(devpath);
        }
        else {
            if (evbits[0] & (1 << EV_FF)) {
                printf("%s supports FF\n", devpath);
                vibrate(fd);
            }
        }
        close(fd);
     }

    (void) closedir (dp);

    return 0;
}
