import colorsys
import logging

from pyhap.accessory import Accessory
from pyhap.const import CATEGORY_LIGHTBULB, CATEGORY_FAN

from hackoort.bulb import Bulb


def hls2rgb(h, l, s):
    """Convert h, l, s in 0-1 range to rgb in 0-255

    :param h: hue
    :param l: luminance
    :param s: saturation
    :return: red, green, blue in 0-255 range
    """
    rgb = colorsys.hls_to_rgb(h, l, s)
    r, g, b = (int(color * 255) for color in rgb)
    return r,g, b


def rgb2hls(r, g, b):
    """Convert r,g,b in 0-255 range to hls in 0.1

    :param r: red
    :param g: green
    :param b: blue
    :return: hue, luminance, saturation
    """
    return colorsys.rgb_to_hls(r/255.0, g/255.0, b/255.0)


class OortColorBulb(Accessory):

    category = CATEGORY_LIGHTBULB

    def __init__(self, driver, name, bulb: Bulb):
        """

        :param driver: pyhap driver
        :param name: descriptive name
        :param bulb: it has to be connected oort bulb
        """
        super().__init__(driver, name)
        self.status = bulb.status
        self.hue, _, self.saturation = rgb2hls(
            self.status.red, self.status.green, self.status.blue)

        serv_light = self.add_preload_service(
            'Lightbulb', chars=["On", "Brightness", "Hue", "Saturation"]
        )
        self.char_on = serv_light.configure_char(
            'On', setter_callback=self.set_on, value=self.status.on,
            getter_callback=self.get_on
        )

        self.char_brightness = serv_light.configure_char(
            "Brightness", setter_callback=self.set_brightness,
            value=self.status.brightness, getter_callback=self.get_brightness
        )
        self.char_brightness = serv_light.configure_char(
            "Hue", setter_callback=self.set_hue,
        )
        self.char_brightness = serv_light.configure_char(
            "Saturation", setter_callback=self.set_saturation,
        )

        self.bulb = bulb

    def get_on(self):
        return self.status.on

    def set_on(self, value):
        # logging.info("Setting bulb: %s", value)
        self.bulb.onoff(value)
        # if value and self.bulb.status.rgbon:
        #     self.bulb.set_rgb_onoff(0)

    def get_brightness(self):
        return self.status.brightness

    def set_brightness(self, value):
        """
        The corresponding value is an integer representing a percentage
        of the maximum brightness.
        :param value:
        :return:
        """
        # logging.info("Setting brightness value: %s", value)
        self.bulb.set_brightness_pct(value)

    def set_hue(self, value):
        """
        The corresponding value is a floating point number in units
        of arc degrees. Values range from 0 to 360, representing the color
        spectrum starting from red, through yellow, green, cyan, blue,
        and finally magenta, before wrapping back to red.
        """
        self.hue = value/360.0
        self.bulb.set_rgb(*hls2rgb(self.hue, 0.5, self.saturation))
        logging.info("Hue: %s", value)

    def set_saturation(self, value):
        """
        The corresponding value is a percentage of maximum saturation.
        :param value:
        :return:
        """
        self.saturation = value / 100.0
        logging.info("Saturation: %s", value)

    def stop(self):
        self.bulb.disconnect()


class OortColorBulbSwitch(Accessory):
    category = CATEGORY_FAN

    def __init__(self, driver, name, bulb: Bulb):
        """

        :param driver: pyhap driver
        :param name: descriptive name
        :param bulb: it has to be connected oort bulb
        """
        super().__init__(driver, name)
        self.status = bulb.status
        self.hue, _, self.saturation = rgb2hls(
            self.status.red, self.status.green, self.status.blue)

        serv_light = self.add_preload_service(
            'Fan', chars=["On", "RotationDirection", "RotationSpeed"]
        )
        self.char_on = serv_light.configure_char(
            'On', setter_callback=self.set_fake_on,
            value=1, getter_callback=self.get_fake_on
        )

        self.char_color_on = serv_light.configure_char(
            'RotationDirection', setter_callback=self.set_color_on,
            value=self.status.on,
            getter_callback=self.get_color_on
        )

        self.char_temperature = serv_light.configure_char(
            "RotationSpeed", setter_callback=self.set_temperature,
            value=self.status.brightness, getter_callback=self.get_temperature
        )

        self.bulb = bulb

    def get_fake_on(value):
        return 1

    def set_fake_on(self, value):
        pass

    def get_color_on(self):
        return self.status.rgbon

    def set_color_on(self, value):
        self.bulb.set_rgb_onoff(value)

    def get_temperature(self):
        return self.status.temperature

    def set_temperature(self, value):
        self.bulb.set_temperature_pct(value)