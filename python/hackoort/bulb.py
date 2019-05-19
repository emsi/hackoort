from gattlib import gattlib

from hackoort import (
    HackoortContext, check_lock_status, get_characteristics, onoff,
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

    def _check_connection(self):
        if self.connection is None:
            raise OortException("Bulb is not connected")

    def connect(self):
        conection = gattlib.gattlib_connect(None, self.bt_address, 1, 1, 0, 0)
        if not conection:
            raise OortException("Cannot connect to BT device")
        self.context.connection = self.connection = conection

        # unlock with password
        if unlock(self.context) != 0:
            raise OortException("Problem sending password")

        if not check_lock_status(self.context):
            raise OortException("Wrong bulb password")

        return self

    def disconnect(self):
        gattlib.gattlib_disconnect(self.connection)
        return self

    def onoff(self, on):
        onoff(self.context, on)
        return self

    def on(self):
        self.onoff(1)

    def off(self):
        self.onoff(0)

    def bw(self):
        self.set_rgb_onoff(0)

    def color(self):
        self.set_rgb_onoff(1)

    def set_brightness(self, brightness):
        set_brightness(self.context, brightness)
        return self

    def set_brightness_pct(self, percent):
        set_brightness_pct(self.context, percent)
        return self

    def set_temperature(self, temperature):
        set_temperature(self.context, temperature)
        return self

    def set_temperature_pct(self, percent):
        set_temperature_pct(self.context, percent)
        return self

    def set_rgb(self, red, green, blue):
        set_rgb(self.context, red, green, blue)
        return self

    def set_rgb_onoff(self, on):
        set_rgb_onoff(self.context, on)
        return self

    def get_status(self):
        return get_characteristics(self.context)
