from gattlib import gattlib
from hackoort import (
    BulbStatus, HackoortContext, check_lock_status, get_characteristics, onoff,
    set_brightness, set_brightness_pct, set_rgb, set_rgb_onoff, set_temperature,
    set_temperature_pct, unlock,
)
from hackoort.errors import OortException


def _checkzero(zero):
    if zero != 0:
        raise OortException("Error comunicating with device")


class Bulb:
    def __init__(self, bt_address, password=b'D000000', verbose=1):
        self.bt_address = bt_address
        self.context = HackoortContext(
            seq=0, verbose=verbose, force=0, dry_run=0, password=password
        )
        self.connection = None
        self.status = None

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

        self.get_status()
        return self

    def disconnect(self):
        gattlib.gattlib_disconnect(self.connection)
        return self

    def onoff(self, on):
        _checkzero(onoff(self.context, on))
        self.status.on = on
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
        _checkzero(set_brightness(self.context, brightness))
        return self

    def set_brightness_pct(self, percent):
        _checkzero(set_brightness_pct(self.context, percent))
        return self

    def set_temperature(self, temperature):
        _checkzero(set_temperature(self.context, temperature))
        return self

    def set_temperature_pct(self, percent):
        _checkzero(set_temperature_pct(self.context, percent))
        return self

    def set_rgb(self, red, green, blue):
        _checkzero(set_rgb(self.context, red, green, blue))
        return self

    def set_rgb_onoff(self, on):
        _checkzero(set_rgb_onoff(self.context, on))
        return self

    def get_status(self) -> BulbStatus:
        self.status = get_characteristics(self.context)
        return self.status
