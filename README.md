# hackoort
Tool and library to interact with OORT Light bulbs

Prerequisites
=============
Od Dbian/Ubuntu based systems (including raspberry pi)
```
apt-get install build-essential cmake libbluetooth-dev libreadline-dev libglib2.0-dev
git clone https://github.com/labapart/gattlib
cd gattlib
mkdir build && cd build
cmake ..
make
cpack -G DEB ..
sudo dpkg -i ./gattlib_0.2_armv7l.deb 
```

Build Hackoort
=============

```
git clone https://github.com/emsi/hackoort
cd hackoort
mkdir build && cd build
cmake ..
make
```

Using oorttool
==============

Check supported options and commands:
```
$ ./oorttool/oorttool -h
USAGE: hackoort [OPTION] COMMAND [command arguments]

Commands:
  ON                    Turn the OORT device ON
  OFF                   Turn the OORT device OFF
  BRIGHTNESS pct        Set the OORT bulb brightness to 'pct' percents
  TEMPERATURE pct       Set the OORT bulb relative temperature to 'pct' percents
  RGB rrggbb            Set the OORT bulb color to 'rrggbb'
  RGB ON/OFF            Set the OORT bulb color mode ON/OFF
  STATUS                Read device status

Options:
  -d, --devide_address ADDR    OORT Device address
  -d, --devide NAME            OORT Device name (from config file)
  -p, --password PASSWOD       Unlock the OORT device using PASSWORD
                               
  -f, --force                  Try to force operation
  -v, --verbose LEVEL          Be verbose (print more debug)
  --dry-run                    Dry run (do not communicat eover BT)
  -h, --help                   Print this help message
```
Make config file to call device by name:
```
cat >> ~/.oorttool.conf << EOF
[desk]
bt_address=84:EB:18:7D:F8:4F
[kitchen]
bt_address=84:EB:18:7D:F8:61
password=qpqp01
description=Kitchen bulb
EOF
```
Check bulb status
```
./oorttool/oorttool -d kitchen STATUS
BULB IS ON BRIGHNESS 100% TEMPERATURE 100%
```
Turn on the bulb
```
$ ./oorttool/oorttool  -d 84:eb:18:7d:f8:4f ON
Unlocked: 00
Unlocked: 01
Setting device ON
BUG WORKAROUND, repeating first command
Setting device ON
```
Set the luminance to 30% and be quiet
```
$ ./oorttool/oorttool -v 0 -d 84:eb:18:7d:f8:4f BRIGHTNESS 30
```
You can chain multiple commands:
```
 ./oorttool/oorttool  -d 84:eb:18:7d:f8:4f ON BRIGHTNESS 90 RGB 22FFEE
Unlocked: 00
Unlocked: 01
Setting device ON
BUG WORKAROUND, repeating first command
Setting device ON
Setting luminance to 0a
Setting RGB to 22ffff
```
Turn off the bulb and show all the debug output
```
$ ./oorttool/oorttool -d 84:eb:18:7d:f8:4f -v 4 OFF
CMD: OFF, arg[last]: OFF, argc: 0
Unlocked: 00
Unlocked: 01
EXECUTING COMMAND: OFF
Setting device OFF
CMD: 0a01, data: 00, SEQ: 01, SUM: 27
RAW data: aa0afc3a86000a010001270d EOF datalen: 1
BUG WORKAROUND, repeating first command
EXECUTING COMMAND: OFF
Setting device OFF
CMD: 0a01, data: 00, SEQ: 02, SUM: 28
RAW data: aa0afc3a86000a010002280d EOF datalen: 1
```

Python bindings
==============
Requires bleeding edge (just c.a. 6 hours old at the time of writing!) python gattlib bindings.
Just checkout from
https://github.com/labapart/gattlib/tree/initial-python-support
(you may expect things to break).

DO NOT ~~pip install gattlib~~!!!

Just checkout gattlib and hackoort and set `PYTHONPATH` like:
```
export PYTHONPATH=/home/emsi/git/hackoort/python/:/home/emsi/git/gattlib/gattlib-py/
```

Interacting with bulb should be as simple as:
```
>>> from hackoort.bulb import Bulb
>>> b = Bulb(bt_address='84:EB:18:7D:F8:61'.encode()).connect()
>>> b.on()
>>> b.off()
>>> b.set_brightness_pct(100)
>>> b.set_rgb(255,30,50)
>>> b.disconnect()
```