from flask_restful import Resource

from oortapi.app import api, bulb


def str2rgb(str_color):
    red = int(str_color[0:2], 16)
    green = int(str_color[2:4], 16)
    blue = int(str_color[4:6], 16)
    return red, green, blue


def rgb2str(red, green, blue):
    return '{:02X}{:02X}{:02X}'.format(red, green, blue)


def check_color(color):
    if color < 0 or color > 255:
        return False
    return True


def check_colors(red, green, blue):
    return check_color(red) and check_color(blue) and check_color(green)


@api.resource('/api/v1/color/<string:color>', '/api/v1/color')
class Color(Resource):
    def get(self, color=None):
        if color is None:
            status = bulb.get_status()
            return rgb2str(status.red, status.green, status.blue)

        colors = str2rgb(color)
        if check_colors(*colors):
            bulb.set_rgb(*colors)

        return color