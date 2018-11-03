/* liboort.h
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
#ifndef __LIBOORT_H__
#define __LIBOORT_H__

typedef struct _hackoortContext hackoortContext;

struct _hackoortContext {
    gatt_connection_t* connection; // gattlib connectio
    unsigned int seq; // oort devices are using sequence numbers for each request
    short verbose,force,dry_run;
    // configuration
    char* bt_address;  // remote device address
    char* password; // default password used when no password is set
    void* context; // pointer to be passed around
    char* name; // remote device descriptive name
};


// Hackoort functions 
int hackoort_check_lock_status(hackoortContext* context);
char* hackoort_read_characteristic(hackoortContext *context);

int hackoort_onoff(hackoortContext* context, unsigned char on);

int hackoort_set_brightness(hackoortContext* context, unsigned char lum);
int hackoort_set_brightness_pct(hackoortContext* context, unsigned char pct);
int hackoort_set_temperature(hackoortContext* context, unsigned char lum);
int hackoort_set_temperature_pct(hackoortContext* context, unsigned char pct);
int hackoort_set_rgb(hackoortContext* context, char r, char g, char b);
int hackoort_set_rgb_onoff(hackoortContext* context, unsigned on);

#endif /* __LIBOORT_H__ */
