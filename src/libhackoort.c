/* libhackoort.c
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
//#include <stdio.h>
//#include <getopt.h>
//#include <stdlib.h>
#include <endian.h>

#include "gattlib.h"
#include "hackoort.h"

typedef char hackoort_cmd[2];

int hackoort_check_lock_status(hackoortContext* context)
{
	int len;
	unsigned char ret;
	static bt_uuid_t g_uuid;

	/* read pass status, handle 0x2a */
	bt_string_to_uuid(&g_uuid, "a8b3fff4-4834-4051-89d0-3de95cddd318"); 
	len = gattlib_read_char_by_uuid(context->connection, &g_uuid, &ret, sizeof(ret));
	if (context->verbose) printf("Unlocked: %02x\n", ret);
	return ret;
}

char* hackoort_read_characteristic(hackoortContext *context)
{
	int len;
	static unsigned char ret[5]="\x00\x00\x00\x00\x00";
	static bt_uuid_t g_uuid;

	/* read pass status, handle 0x2a */
	bt_string_to_uuid(&g_uuid, "a8b3fff2-4834-4051-89d0-3de95cddd318");
	len = gattlib_read_char_by_uuid(context->connection, &g_uuid, &ret, sizeof(ret));
	return ret;
}

int aa0afc3a8600(hackoortContext* context, hackoort_cmd cmd, char* data, int datalen)
{
	unsigned char buffer[]="\xaa\x0a\xfc\x3a\x86\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00";
	int i,j,ret,len=12;
	unsigned char sum=0xab;

	buffer[6]=cmd[0];
	buffer[7]=cmd[1];

	for (i=0;i<datalen;i++)
	    buffer[8+i]=((char *)data)[i];
	i=i+++8;  // this is plain evil ;)
	buffer[i++]=context->seq; // Sequence number
	
	for (j=0;j<i;j++) {
			sum+=buffer[j];
	}
	buffer[i++]=sum; // checksum
	buffer[i++]='\x0d'; // terminator?

	if (context->verbose>1) {
	    printf("CMD: %02x%02x, data:", cmd[0],cmd[1]);
	    for (j=0;j<datalen;j++)
		printf(" %02x", data[j]);
	    printf(", SEQ: %02x, SUM: %02x\n", context->seq, sum);
	}
	if (context->verbose>3) {
	    printf("RAW data: ");
	    for (j=0;j<i;j++) 
		printf("%02x", buffer[j]);
	    printf(" EOF datalen: %u\n",datalen);
	}


	/* handle 0x21 */
	if (!context->dry_run) {
	    ret = gattlib_write_char_by_handle(context->connection, 0x21, buffer, i);
	    assert(ret == 0);
	}
	if (!context->seq) {
	    if (context->verbose) printf("BUG WORKAROUND, repeating first command\n");
	    context->seq++;
	    aa0afc3a8600(context, cmd, data, datalen);
	    context->seq--;
	}
	context->seq++;

	return ret;
}

int hackoort_onoff(hackoortContext* context, unsigned char on)
{
    if (context->verbose) {
	printf("Setting device %s\n",on?"ON":"OFF");
    }
    return aa0afc3a8600(context, "\x0a\x01", (void*) &on, 1);
}

int hackoort_set_luminance(hackoortContext* context, unsigned char lum)
{
    if (context->verbose) printf ("Setting luminance to %02x\n", lum);
    return aa0afc3a8600(context, "\x0c\x01", (void*) &lum, 1);
}

int hackoort_set_luminance_pct(hackoortContext* context, unsigned char pct)
{
    unsigned char lum=2+(9.0*(pct>100?100:pct)/100.0);
    return hackoort_set_luminance(context, lum);
}

int hackoort_set_rgb(hackoortContext* context, char r, char g, char b)
{
    char data[6]="\x00\x00\x00\x00\x00\x00";
    data[0]=1; // RGB mode
    data[1]=r;
    data[2]=g;
    data[3]=b;
    if (context->verbose) {
	printf("Setting RGB to %02x%02x%02x\n",r,g,b);
    }
    return aa0afc3a8600(context, "\x0d\x06", (void*) data, 6);
}

int hackoort_set_rgb_onoff(hackoortContext* context, unsigned on)
{
    char data[6]="\x00\x00\x00\x00\x00\x00";
    unsigned char* characteristic=hackoort_read_characteristic(context);
    data[0]=on?1:2; // RGB mode
    data[1]=characteristic[2];
    data[2]=characteristic[3];
    data[3]=characteristic[4];
    if (context->verbose) {
	printf("Setting RGB %s, color %02x%02x%02x\n",on?"ON":"OFF",characteristic[2],characteristic[3],characteristic[4]);
    }
    return aa0afc3a8600(context, "\x0d\x06", (void*) data, 6);
}
