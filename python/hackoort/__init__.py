from _ctypes import POINTER
from ctypes import (
    CDLL, Structure, c_char_p, c_short, c_ubyte, c_uint, c_void_p,
)

from gattlib import gattlib

libhackoort = CDLL("libhackoort.so")


# typedef struct _hackoortContext hackoortContext;
#
# struct _hackoortContext {
#     gatt_connection_t* connection; // gattlib connectio
#     unsigned int seq; // oort devices are using sequence numbers for each request
#     short verbose,force,dry_run;
#     // configuration
#     char* bt_address;  // remote device address
#     char* password; // default password used when no password is set
#     void* context; // pointer to be passed around
#     char* name; // remote device descriptive name
# };
class HackoortContext(Structure):
    _fields_ = [
        ('connection', c_void_p),
        ('seq', c_uint),
        ('verbose', c_short),
        ('force', c_short),
        ('dry_run', c_short),
        ('bt_address', c_char_p),
        ('password', c_char_p),
        ('context', c_void_p),
        ('name', c_char_p),
    ]


# hackoortContext context = {
#     NULL, // connection
#     0, // seq
#     1, // verbose
#     0, // force
#     0, // dry run
#     "84:eb:18:7d:f8:4f", // bt_address
#     "D000000", // password
#     NULL
# };
_context = HackoortContext(
    seq=0, verbose=4, force=0, dry_run=0, password=b'D000000'
)


# struct _bulbStatus {
#     unsigned char on;
#     unsigned char brightness;
#     unsigned char temperature;
#     unsigned char rgbon;
#     unsigned char red;
#     unsigned char green;
#     unsigned char blue;
# };
class BulbStatus(Structure):
    _fields_ = [
        ('on', c_ubyte),
        ('brightness', c_ubyte),
        ('temperature', c_ubyte),
        ('rgbon', c_ubyte),
        ('red', c_ubyte),
        ('green', c_ubyte),
        ('blue', c_ubyte),
    ]


def unlock(context: HackoortContext):
    return gattlib.gattlib_write_char_by_handle(
        context.connection, 0x27, context.password, len(context.password)
    )


# int hackoort_check_lock_status(hackoortContext* context)
check_lock_status = libhackoort.hackoort_check_lock_status
check_lock_status.argtypes = [POINTER(HackoortContext)]

# int hackoort_onoff(hackoortContext* context, unsigned char on);
onoff = libhackoort.hackoort_onoff
onoff.argtypes = [POINTER(HackoortContext), c_ubyte]

# int hackoort_set_brightness(hackoortContext* context, unsigned char lum);
set_brightness = libhackoort.hackoort_set_brightness
set_brightness.argtypes = [POINTER(HackoortContext), c_ubyte]

# int hackoort_set_brightness_pct(hackoortContext* context, unsigned char pct);
set_brightness_pct = libhackoort.hackoort_set_brightness_pct
set_brightness_pct.argtypes = [POINTER(HackoortContext), c_ubyte]


# int hackoort_set_temperature(hackoortContext* context, unsigned char lum);
set_temperature = libhackoort.hackoort_set_temperature
set_temperature.argtypes = [POINTER(HackoortContext), c_ubyte]

# int hackoort_set_temperature_pct(hackoortContext* context, unsigned char pct);
set_temperature_pct = libhackoort.hackoort_set_temperature_pct
set_temperature_pct.argtypes = [POINTER(HackoortContext), c_ubyte]

# int hackoort_set_rgb(hackoortContext* context, char r, char g, char b);
set_rgb = libhackoort.hackoort_set_rgb
set_rgb.argtypes = [POINTER(HackoortContext), c_ubyte, c_ubyte, c_ubyte]

# int hackoort_set_rgb_onoff(hackoortContext* context, unsigned on);
set_rgb_onoff = libhackoort.hackoort_set_rgb_onoff
set_rgb_onoff.argtypes = [POINTER(HackoortContext), c_ubyte]

# char* hackoort_read_characteristic(hackoortContext *context);
read_characteristic = libhackoort.hackoort_read_characteristic
read_characteristic.restype = c_char_p
read_characteristic.argtypes = [POINTER(HackoortContext)]

# bulbStatus hackoort_get_characteristics(hackoortContext *context);
get_characteristics = libhackoort.hackoort_get_characteristics
get_characteristics.restype = BulbStatus
get_characteristics.argtypes = [POINTER(HackoortContext)]