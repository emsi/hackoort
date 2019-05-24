import logging

from pyhap.accessory import Accessory
from pyhap.const import CATEGORY_LIGHTBULB

from hackoort.bulb import Bulb


class OortBulb(Accessory):

    category = CATEGORY_LIGHTBULB

    def __init__(
        self, driver, name, bt_address, password=b'D000000', verbose=0
    ):
        super().__init__(driver, name)
        serv_light = self.add_preload_service(
            'Lightbulb', chars=["On", "Brightness", "Hue", "Saturation"]
        )
        self.char_on = serv_light.configure_char(
            'On', setter_callback=self.set_bulb)

        self.char_brightness = serv_light.configure_char(
            "Brightness", setter_callback=self.set_brightness,
        )
        self.char_brightness = serv_light.configure_char(
            "Hue", setter_callback=self.set_brightness,
        )
        self.char_brightness = serv_light.configure_char(
            "Saturation", setter_callback=self.set_brightness,
        )

        self.bulb = Bulb(
            bt_address, password=password, verbose=verbose
        ).connect()

    def __getstate__(self):
        state = super().__getstate__()
        # state['sensor'] = None
        return state

    def __setstate__(self, state):
        logging.info("state: %s", state)

    def set_bulb(self, value):
        # logging.info("Setting bulb: %s", value)
        self.bulb.onoff(value)

    def set_brightness(self, value):
        # logging.info("brightness value: %s", value)
        self.bulb.set_brightness_pct(value)

    def stop(self):
        self.bulb.disconnect()