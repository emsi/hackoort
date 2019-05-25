"""An example of how to setup and start an Accessory.

This is:
1. Create the Accessory object you want.
2. Add it to an AccessoryDriver, which will advertise it on the local network,
    setup a server to answer client queries, etc.
"""
import logging
import signal

from pyhap.accessory import Bridge
from pyhap.accessory_driver import AccessoryDriver

from hackoort.bulb import Bulb
from oorthap.bulb import OortColorBulb, OortColorBulbSwitch

logging.basicConfig(level=logging.INFO, format="[%(module)s] %(message)s")


def get_bridge(driver):
    """Call this method to get a Bridge instead of a standalone accessory."""
    bridge = Bridge(driver, 'pyBridge')
    bridge.set_info_service(manufacturer="Emsi")

    bulb = Bulb('84:EB:18:7D:F8:61'.encode(), verbose=1).connect()
    bridge.add_accessory(OortColorBulb(driver, 'Corner', bulb))
    bridge.add_accessory(OortColorBulbSwitch(driver, 'Color switch', bulb))
    return bridge


# Start the accessory on port 51826
driver = AccessoryDriver(port=51826, pincode=b"666-77-666")

# Change `get_accessory` to `get_bridge` if you want to run a Bridge.
driver.add_accessory(accessory=get_bridge(driver))

# We want SIGTERM (terminate) to be handled by the driver itself,
# so that it can gracefully stop the accessory, server and advertising.
signal.signal(signal.SIGTERM, driver.signal_handler)

# Start it!
driver.start()
