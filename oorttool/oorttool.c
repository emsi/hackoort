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

#include "gattlib.h"
#include "hackoort.h"

hackoortContext context = {
    NULL,
    1,
    1,
    0,
    0,
    "84:eb:18:7d:f8:4f",
    "D000000"
};

char *command=NULL, **arguments=NULL;
unsigned short arguments_count=0;

typedef char hackoort_cmd[2];

static void usage() {
	printf("USAGE: hackoort [OPTION] COMMAND [command arguments]\n\n");
	printf("Commands:\n");
	printf("  ON                    Turn the OORT device ON\n");
	printf("  OFF                   Turn the OORT device OFF\n");
	printf("  BRIGHTNESS pct	Set the OORT bulb brightness to 'pct' percents\n");
	printf("\n");
	printf("Options:\n"
	"  -d, --devide_address ADDR    OORT Device address\n"
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
			case 'f':
				context.force=~context.force;
				break;
			case 'v':
				if (optarg) 
					context.verbose=strtol(optarg,NULL,10);
				else
					context.verbose=1;
				/*printf("VERBOSE LEVEL  = %i\n",context.verbose);*/
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
	if (context.verbose>3) printf("CMD: %s, arg[last]: %s, argc: %i\n", command, arguments[arguments_count-1], arguments_count);
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

int parse_command(int argc, char** argv)
{
	static unsigned short prime=0;
	unsigned short args=0; // number of arguments for current command including command

	if (context.verbose >2) printf("EXECUTING COMMAND: %s\n", command);

	if (strcmp(command, "ON") == 0)	
		hackoort_onoff(&context, 1);
	else if (strcmp(command, "OFF") == 0)
		hackoort_onoff(&context, 0);
	else if (strcmp(command, "LUMINANCE") == 0) {
		args++;
		hackoort_set_luminance_pct(&context, strtol(arguments[0],NULL,10));
		}
	else if (strcmp(command, "RAW") == 0) {
		unsigned char len=strlen(arguments[1])/2;
		char* cmd=read_hex_data(arguments[0],2);
		char* data=read_hex_data(arguments[1],len);
		args+=2;
		aa0afc3a8600(&context, cmd, data, len);
		free(cmd);free(data);
		}
	if (!prime) { 
		prime=1;
		if (context.verbose) printf("BUG WORKAROUND, repeating first command\n");
		parse_command(argc,argv);
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
	static bt_uuid_t g_uuid;


	parse_opts(argc,argv);

	context.connection = gattlib_connect(NULL, context.bt_address, BDADDR_LE_PUBLIC, BT_IO_SEC_LOW, 0, 0);
	if (context.connection == NULL) {
		fprintf(stderr, "Fail to connect to the bluetooth device.\n");
		return 1;
	}

	/* read pass status */
	hackoort_check_lock_status(&context);

	// char-write-req 27 44303030303030
	ret = gattlib_write_char_by_handle(context.connection, 0x27, context.password, strlen(context.password));
	assert(ret == 0);

	/* read pass status */
	hackoort_check_lock_status(&context);
	
	parse_command(argc, argv);

	gattlib_disconnect(context.connection);
	return 0;
}
