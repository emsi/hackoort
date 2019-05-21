from flask import Flask
from flask_restful import Api

from hackoort.bulb import Bulb

app = Flask(__name__.split('.')[0])
api = Api(app)

bulb = Bulb(bt_address='84:EB:18:7D:F8:61'.encode(), verbose=1).connect()
