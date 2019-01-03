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


#include "pwr.h"

static struct option long_options[] = {
    { "help",    no_argument,       NULL, 'h' },
    { "pretty",  no_argument,       NULL, 'p' },
    { "force",   required_argument, NULL, 'f' },
    { "version", no_argument,       NULL, 'v' },
    { NULL,      0,                 0,    0   }
};

void version() {
    puts("pwr v1.0");
    puts("Copyright (C) 2019 Aidan Williams");
    puts("License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>.");
    puts("This is free software: you are free to change and redistribute it.");
    puts("There is NO WARRANTY, to the extent permitted by law.\n");
    puts("Written by Aidan Williams");
    exit(EXIT_SUCCESS);
}

void usage(char* progpth, bool err) {
    FILE* stream;
    if(err) {
        stream = stderr;
    } else {
        stream = stdout;
    }
    
    printf("Usage: %s [OPTIONS]\n\n", progpth);
    puts("-h, --help  \tPrints this help text");
    puts("-p, --pretty\tPretty prints output");
    puts("-f, --force \tForces path defined by argument given");

    if(err) {
        exit(EXIT_FAILURE);
    } else {
        exit(EXIT_SUCCESS);
    }
};

// Internal for getting power from the path of a sysfs battery
int sysfspwr(const char* path) {
    FILE* fptr;
    int percent = 0;

    fptr = fopen(path, "r");
    if(fptr) {
        fscanf(fptr, "%d", &percent);
        fclose(fptr);
    } else {
        fprintf(stderr, "Error opening file: %s", path);
        exit(EIO);
    }

    return percent;
}

int pwr() {
    glob_t glb;
    glob("/sys/class/power_supply/*/capacity", 0, NULL, &glb);

    int avrg = 0;
    int avgtot = 0;

    // Averages battery
    int i;
    for(i = 0; i < glb.gl_pathc; i++) {
        avgtot += sysfspwr(glb.gl_pathv[i]);
    }

    avrg = avgtot / glb.gl_pathc;

    // Free up memory
    globfree(&glb);

    return avrg;
}

int fpwr(const char* frcpath) {
    return sysfspwr(frcpath);
}

int main(int argc, char **argv) {
    int opt;
    char* fmt = "%d\n";

    while((opt = getopt_long(argc, argv, "pfhv", long_options, NULL)) != -1) {
        switch(opt) {
            case 'p':
                fmt = "%d\%\n";
                break;
            case 'f':
                printf(fmt, fpwr(optarg));
                exit(0);
            case 'h':
                usage(argv[0], false);
            case 'v':
                version();
            default:
                usage(argv[0], true);
        }
    }

    printf(fmt, pwr());
    return 0;
}
