import logging

from gattlib import gattlib
from hackoort import (
    BulbStatus, HackoortContext, check_lock_status, get_characteristics, onoff,
    set_brightness, set_brightness_pct, set_rgb, set_rgb_onoff, set_temperature,
    set_temperature_pct, unlock,
)
from hackoort.errors import OortException


class Bulb:
    def __init__(self, bt_address, password=b'D000000', verbose=1):
        self.bt_address = bt_address
        self.context = HackoortContext(
            seq=0, verbose=verbose, force=0, dry_run=0, password=password
        )
        self.connection = None

    def _checkzero(self, zero):
        if zero != 0:
            # try to reconnect
            logging.warn("Attempting to reconnect.")
            self.connect()
            raise OortException("Error communicating with device")

    def _check_connection(self):
        if self.connection is None:
            raise OortException("Bulb is not connected")

    def connect(self):
        connection = gattlib.gattlib_connect(None, self.bt_address, 1, 1, 0, 0)
        if not connection:
            raise OortException("Cannot connect to BT device")
        self.context.connection = self.connection = connection

        # unlock with password
        if unlock(self.context) != 0:
            raise OortException("Problem sending password")

        if not check_lock_status(self.context):
            raise OortException("Wrong bulb password")

        self.get_status()
        return self

    def disconnect(self):
        if self.connection:
            self.connection = None
            gattlib.gattlib_disconnect(self.connection)
        return self

    def onoff(self, on):
        self._checkzero(onoff(self.context, on))
        return self

    def on(self):
        return self.onoff(1)

    def off(self):
        return self.onoff(0)

    def bw(self):
        return self.set_rgb_onoff(0)

    def color(self):
        return self.set_rgb_onoff(1)

    def set_brightness(self, brightness):
        self._checkzero(set_brightness(self.context, brightness))
        return self

    def set_brightness_pct(self, percent):
        self._checkzero(set_brightness_pct(self.context, percent))
        return self

    def set_temperature(self, temperature):
        self._checkzero(set_temperature(self.context, temperature))
        return self

    def set_temperature_pct(self, percent):
        self._checkzero(set_temperature_pct(self.context, percent))
        return self

    def set_rgb(self, red, green, blue):
        self._checkzero(set_rgb(self.context, red, green, blue))
        return self

    def set_rgb_onoff(self, on):
        self._checkzero(set_rgb_onoff(self.context, on))
        return self

    def get_status(self) -> BulbStatus:
        return get_characteristics(self.context)

    @property
    def status(self) -> BulbStatus:
        return self.get_status()
