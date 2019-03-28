/*
 * Copyright (C) 2019 Aidan Williams
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "config.h"

#include <errno.h>
#include <getopt.h>
#include <glob.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(__FreeBSD__)
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <dev/acpica/acpiio.h>
#include <libexplain/ioctl.h>
#endif

// String printed out with --version
static const char *versionstr =
	"pwr v1.0\n"
	"Copyright (C) 2019 Aidan Williams\n"
	"License GPLv3+: GNU GPL version 3 or later "
	"<https://gnu.org/licenses/gpl.html>\n"
	"This is free software: you are free to change and redistribute it.\n"
	"There is NO WARRANTY, to the extent permitted by law.\n\n"
	"Written by Aidan Williams";

// String printed out with --usage
static const char *usagestr =
	"Usage: %s [OPTIONS]\n\n"
	"-h, --help   \tPrints this help text\n"
	"-v, --version\tPrints the version information\n"
	"-f, --format \tFormats output by argument given\n"
	"-s, --single \tGets power from battery defined by argument given\n";

static struct option long_options[] = {
	{ "help",    no_argument,       0, 'h' },
	{ "format",  required_argument, 0, 'f' },
	{ "single",  required_argument, 0, 's' },
	{ "version", no_argument,       0, 'v' },
	{ 0,         0,                 0, 0   }
};

// Prints usage
void usage(char *progpth, int err)
{
	// Note that err is expected to be 0 when succsessful
	fprintf(err ? stderr : stdout, usagestr, progpth);
	exit(err);
}

#if defined(__linux)
// Internal for getting power from the path of a sysfs battery
int sysfspwr(const char *path)
{
	FILE *fptr;
	int percent = 0;
	
	if ((fptr = fopen(path, "r"))) {
		fscanf(fptr, "%d", &percent);
		fclose(fptr);
	} else {
		fprintf(stderr, "%s: %s\n", strerror(errno), path);
		exit(errno);
	}

	return percent;
}
#endif

#if defined(__FreeBSD__)
// Internal for getting power from the id of a battery
int bsdpwr(int bat)
{
	static int acpi;
	
	// Try opening ACPI kernel interface
	acpi = open("/dev/acpi", O_RDWR);
	if (acpi < 0)
		acpi = open("/dev/acpi", O_RDONLY);
	
	if (acpi < 0) {
		fprintf(stderr, "%s: %s\n", strerror(errno), "/dev/acpi");
		exit(errno);
	}
	
	// Ask kernel nicely for battery information
	union acpi_battery_ioctl_arg battio;
	battio.unit = bat;
	
	if (ioctl(acpi, ACPIIO_BATT_GET_BATTINFO, &battio) < 0) {
		fprintf(stderr, "ioctl error for battery %i: %s\n",bat, explain_ioctl(acpi, ACPIIO_BATT_GET_BATTINFO, &battio));
		exit(EXIT_FAILURE);
	}
	
	int percent = battio.battinfo.cap;
	
	close(acpi);
	return percent;
}
#endif

// Gets the average power of all of the system batteries
int pwr()
{
#if defined(__linux)
	glob_t glb;

	// GLOB_NOSORT is for speed
	glob("/sys/class/power_supply/*/capacity", GLOB_NOSORT, 0, &glb);

	// glb.gl_pathc is truthy even when it's a nonzero number
	if (glb.gl_pathc < 1) {
		globfree(&glb);
		fputs("No batteries found\n", stderr);
		exit(ENODEV);
	}

	int avrg = 0;
	int avgtot = 0;
	
	// Averages battery
	for (size_t i = 0; i < glb.gl_pathc; i++)
		avgtot += sysfspwr(glb.gl_pathv[i]);

	avrg = avgtot / glb.gl_pathc;

	globfree(&glb);
	return avrg;
#elif defined(__FreeBSD__)
	return bsdpwr(ACPI_BATTERY_ALL_UNITS);
#else
	fputs("Unsupported platform\n", stderr);
	exit(EXIT_FAILURE);
#endif
}


// Gets the power of the specified battery
int fpwr(const char *bat)
{
#if defined(__linux)
	// TODO: Find a cleaner way to do this
	char *tmp = malloc(
		sizeof("/sys/class/power_supply/BAT/capacity") 
		+ sizeof(bat) 
		- 1 // Account for null byte at end of strings
	);

	sprintf(tmp, "/sys/class/power_supply/BAT%s/capacity", bat);

	int batpwr = sysfspwr(tmp);
	free(tmp);
	
	return batpwr;
#elif defined(__FreeBSD__)
	return bsdpwr(atoi(bat));
#else	
	fputs("Unsupported platform\n", stderr);
	exit(EXIT_FAILURE);
#endif
}


int main(int argc, char **argv)
{
	int opt;
	char *battery = NULL;
	char *pwrfmt = PWRFORMAT;
	
	while ((opt = getopt_long(argc, argv, "mf:s:hv", long_options, 0)) != -1) {
		switch (opt) {
		case 'f':
			pwrfmt = optarg;
			break;
		case 's':
			battery = optarg;
			break;
		case 'h':
			usage(argv[0], 0);
		case 'v':
			puts(versionstr);
			return 0;
		default:
			usage(argv[0], EINVAL);
		}
	}
	printf(pwrfmt, battery ? fpwr(battery) : pwr());
	return 0;
}
