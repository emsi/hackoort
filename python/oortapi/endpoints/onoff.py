from flask_restful import Resource

from oortapi.app import api, bulb


@api.resource('/api/v1/on')
class On(Resource):
    def get(self):
        # set to BW first
        bulb.bw().on()
        return 1


@api.resource('/api/v1/off')
class Off(Resource):
    def get(self):
        bulb.off()
        return 0
