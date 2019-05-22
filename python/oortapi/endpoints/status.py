from flask_restful import Resource

from oortapi.app import api, bulb


@api.resource('/api/v1/status')
class Status(Resource):
    def get(self):
        status = bulb.get_status()
        return status.on


@api.resource('/api/v1/brightness/<int:lum>', '/api/v1/brightness')
class Brightness(Resource):
    def get(self, lum=None):
        if lum is None:
            status = bulb.get_status()
            return status.brightness

        if lum <= 0 or lum  > 100:
            return 0

        if not bulb.status.on:
            bulb.on()

        bulb.set_brightness_pct(lum)
        return lum
