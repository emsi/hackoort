/* hackoort.c
 *
 * Copyright (C) 2016 Mariusz Woloszyn <emsi@nosuchdomain.example>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <assert.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <endian.h>
#include <glib.h>

#include "gattlib.h"
#include "hackoort.h"

const char* CONFIG_FILE=".oorttool.conf";

hackoortContext context = {
    NULL, // connection
    0, // seq
    1, // verbose
    0, // force
    0, // dry run
    "84:eb:18:7d:f8:4f", // bt_address
    "D000000", // password
    NULL
};

typedef struct {
    char* name;
    char* bt_address;
    char* password;
    char* description;
} Devices;

Devices** devices=NULL;

char *command=NULL, **arguments=NULL;
unsigned short arguments_count=0;

typedef char hackoort_cmd[2];

int read_config() {
    GKeyFile* gkf;
    GError *error = NULL;
    gchar **groups;
    gsize glen=0;

    char *path;

    path = g_build_filename (g_get_home_dir (), CONFIG_FILE, NULL);

    gkf = g_key_file_new();
    if (!g_key_file_load_from_file(gkf, path, G_KEY_FILE_NONE, &error)){
	fprintf (stderr, "Could not read config file %s\n", CONFIG_FILE);
	//g_error (error->message);
	g_free (path);
	return EXIT_FAILURE;
    }

    groups=g_key_file_get_groups(gkf, &glen);
    if (context.verbose>3) {
	printf("Parsign config %s\n", path);
	printf("  devices: %i\n",glen);
    }
    if (glen) {
	devices=calloc(glen,sizeof(Devices*));
	int i=0;
	for (;i<glen;i++) {
	    devices[i] = g_slice_new (Devices);
	    devices[i]->name=groups[i];
	    devices[i]->bt_address=g_key_file_get_string(gkf, groups[i], "bt_address", NULL);
	    devices[i]->password=g_key_file_get_string(gkf, groups[i], "password", NULL);
	    devices[i]->description=g_key_file_get_string(gkf, groups[i], "description", NULL);

	    /* set global context if device name matches the device in config */
	    if (!strcmp(groups[i],context.bt_address)) {
		context.bt_address=devices[i]->bt_address;
		context.password=devices[i]->password;
		context.name=groups[i];
		}
	    if (context.verbose>3) {
		printf ("  [%s]\n",groups[i]);
		printf ("    %s: (%s) %s\n",devices[i]->bt_address, devices[i]->password, devices[i]->description);
	    }
	}
    }
    /* g_strfreev(groups); // we'll be using it */
    g_free (path);
    g_key_file_free (gkf);
}

static void usage() {
	printf("USAGE: hackoort [OPTION] COMMAND [command arguments]\n\n");
	printf("Commands:\n");
	printf("  ON                    Turn the OORT device ON\n");
	printf("  OFF                   Turn the OORT device OFF\n");
	printf("  BRIGHTNESS pct        Set the OORT bulb brightness to 'pct' percents\n");
	printf("  TEMPERATURE pct       Set the OORT bulb relative temperature to 'pct' percents\n");
	printf("  RGB rrggbb            Set the OORT bulb color to 'rrggbb'\n");
	printf("  RGB ON/OFF            Set the OORT bulb color mode ON/OFF\n");
	printf("  STATUS                Read device status\n");
	printf("\n");
	printf("Options:\n"
	"  -d, --device_address ADDR    OORT Device address\n"
	"  -d, --device NAME            OORT Device name (from config file)\n"
	"  -p, --password PASSWOD       Unlock the OORT device using PASSWORD\n"
	"                               \n"
	"  -f, --force                  Try to force operation\n"
	"  -v, --verbose LEVEL          Be verbose (print more debug)\n"
	"  --dry-run                    Dry run (do not communicat eover BT)\n"
	"  -h, --help                   Print this help message\n"
	"\n");
	exit (-1);
}

int parse_opts(int argc, char **argv)
{
	int option_index = 0,opt,i;
	static struct option loptions[] = {
		{"help",0,0,'h'},
		{"device_address",1,0,'d'},
		{"device",1,0,'d'},
		{"password",1,0,'p'},
		{"force",0,0,'f'},
		{"verbose",2,0,'v'},
		{"dry-run",0,0,0},
		{0,0,0,0}
	};
	while ((opt = getopt_long(argc,argv,"hd:p:fv:", loptions, &option_index))!=-1) {
		switch (opt) {
			case 0:
			    if (!(strcmp("dry-run", loptions[option_index].name)))
				context.dry_run=1;
			    break;
			case 'f':
				context.force=~context.force;
				break;
			case 'v':
				if (optarg) 
					context.verbose=strtol(optarg,NULL,10);
				else
					context.verbose=1;
				//printf("VERBOSE LEVEL  = %i\n",context.verbose);
				break;
			case 'd':
				context.bt_address=strdup(optarg);
				break;
			case 'h':
				usage();
				break;
		}
	}
	if (optind<=argc) {
		command=argv[optind];
	}
	if (optind++<=argc) {
		arguments=&(argv[optind]);
		arguments_count=argc-optind;
	}
	if (command && context.verbose>3) printf("CMD: %s, arg[last]: %s, argc: %i\n", command, arguments[arguments_count-1], arguments_count);
}			

// Take string input like 0x1222 or 0a0406 and return bufer with bytes of such value
char* read_hex_data(char* str,unsigned char len)
{
    static uint64_t num;
    long unsigned l=0;
    l=sscanf(str, "%12llx", &num);
    num=htole64(num);
    if (context.verbose>4) printf("HEX: 0x%llx l: %lx \n", num, l);
    char* const buff = (char*)malloc(len);
    memcpy(buff,&num,len);
    return buff;
}

void print_status(unsigned char* status) {
    unsigned char on, brightness, temperature, rgbon, r, g, b;

    on=status[0]&&1;
    brightness=(status[1]&0xf0)>>4;
    temperature=status[1]&0xf;
    rgbon=(temperature==0);
    r=status[2];
    g=status[3];
    b=status[4];
    // convert brightness and luminance to %
    brightness=(brightness-1)*10;
    temperature=(temperature-1)*10;

    printf("The BULB %s is %s. BRIGHNESS %u\%%. ",context.name, on?"ON":"OFF", brightness);
    if (rgbon) printf ("COLOR %02x%02x%02x.\n", r,g,b);
    else printf ("TEMPERATURE %u\%%.\n", temperature);

}

int parse_command(int argc, char** argv)
{
	unsigned short args=0; // number of arguments for current command including command

	if (context.verbose >2) printf("EXECUTING COMMAND: %s\n", command);

	if (strcmp(command, "ON") == 0)	
		hackoort_onoff(&context, 1);
	else if (strcmp(command, "OFF") == 0)
		hackoort_onoff(&context, 0);
	else if (!strcmp(command, "BRIGHTNESS")) {
		args++;
		hackoort_set_brightness_pct(&context, strtol(arguments[0],NULL,10));
		}
	else if (!strcmp(command, "B")) {
		args++;
		hackoort_set_brightness(&context, strtol(arguments[0],NULL,10));
		}
	else if (!strcmp(command, "TEMPERATURE")) {
		args++;
		hackoort_set_temperature_pct(&context, strtol(arguments[0],NULL,10));
		}
	else if (!strcmp(command, "T")) {
		args++;
		hackoort_set_temperature(&context, strtol(arguments[0],NULL,10));
		}
	else if (strcmp(command, "RGB") == 0) {
		args++;
		unsigned char on=0xff;
		if (!strcmp(arguments[0],"ON"))
		    on=1;
		else if (!strcmp(arguments[0],"OFF"))
		    on=0;
		if (on!=0xff) {
		    hackoort_set_rgb_onoff(&context,on);
		} else {
		    char* rgb=read_hex_data(arguments[0],3);
		    hackoort_set_rgb(&context,rgb[2],rgb[1],rgb[0]);
		    free(rgb);
		}
	}
	else if (strcmp(command, "RAW") == 0) {
		unsigned char len=strlen(arguments[1])/2;
		char* cmd=read_hex_data(arguments[0],2);
		char* data=read_hex_data(arguments[1],len);
		args+=2;
		aa0afc3a8600(&context, cmd, data, len);
		free(cmd);free(data);
	}
	else if (strcmp(command, "STATUS") == 0) {
	    unsigned char* ret=hackoort_read_characteristic(&context);
	    print_status(ret);
	    if (context.verbose>1) {
		int i;
		printf("RAW Characteristic ");
		for (i = 0; i < 5; i++)
		   printf("%02x ", ret[i]);
		printf("\n");
	    }
	}

	optind+=args;
	// Execute next command in line if any
	if (optind<argc) {
		command=argv[optind];
		if (optind++<=argc) {
			arguments=&(argv[optind]);
			arguments_count=argc-optind;
		}
		parse_command(argc,argv);
	}
}


int main(int argc, char *argv[]) {
	uint8_t buffer[100];
	int i, len, ret;
	static uuid_t g_uuid;

	parse_opts(argc,argv);

	read_config();

	if (!command) goto out;

	if (context.verbose>1) 
	    printf("Connecting to %s\n",context.bt_address);
	context.connection = gattlib_connect(NULL, context.bt_address, BDADDR_LE_PUBLIC, BT_SEC_LOW, 0, 0);
	if (context.connection == NULL) {
		fprintf(stderr, "Failed to connect to the bluetooth device %s.\n", context.bt_address);
		return 1;
	}
	if (context.verbose>2) 
	    printf("Connected to %s\n",context.bt_address);

	/* read pass status */
	// hackoort_check_lock_status(&context);

	// char-write-req 27 44303030303030  - unlock device 
	if (!context.password) {
	    context.password="D000000";
	}
	ret = gattlib_write_char_by_handle(context.connection, 0x27, context.password, strlen(context.password));
        assert(ret == 0);

	/* read pass status */
	hackoort_check_lock_status(&context);
	
	if (command) parse_command(argc, argv);

	gattlib_disconnect(context.connection);
out:
	return 0;
}
