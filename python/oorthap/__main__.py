import logging
import signal

from pyhap.accessory import Bridge
from pyhap.accessory_driver import AccessoryDriver

from hackoort.bulb import Bulb
from oorthap.bulb import OortColorBulb, OortColorBulbSwitch

logging.basicConfig(level=logging.INFO, format="[%(module)s] %(message)s")

OORT_CONFIG = "/etc/oort/oort.yaml"


def load_config():
    from ruamel import yaml

    with open(OORT_CONFIG, 'r') as fp:
        yaml = yaml.YAML(typ='safe')
        config = yaml.load(fp)

    config['verbose'] = config.get('verbose') or 0
    config['port'] = config.get('port') or 51826
    config['bridge_name'] = config.get('bridge_name') or 'oort'

    return config


def get_bridge(driver):
    """Call this method to get a Bridge instead of a standalone accessory."""
    bridge = Bridge(driver, config['bridge_name'])
    bridge.set_info_service(manufacturer="Emsi")

    devices = config['devices']
    for device in devices:
        password = devices[device].get('password') or b'D000000'
        bulb = Bulb(
            devices[device]['address'].encode(), password=password,
            verbose=config['verbose']
        ).connect()
        name = devices[device].get('name') or device
        bridge.add_accessory(OortColorBulb(driver, name, bulb))
        if devices[device]['color_switch'] == 'yes':
            bridge.add_accessory(OortColorBulbSwitch(
                driver, '{} color switch'.format(device), bulb
            ))
    return bridge


config = load_config()
print(config)

# Start the accessory on port 51826
driver = AccessoryDriver(
    port=config['port'],
    persist_file='/etc/oort/accessory.state'
)

# Change `get_accessory` to `get_bridge` if you want to run a Bridge.
driver.add_accessory(accessory=get_bridge(driver))

logging.info("Pairing PIN: {}".format(driver.state.pincode.decode()))

# We want SIGTERM (terminate) to be handled by the driver itself,
# so that it can gracefully stop the accessory, server and advertising.
signal.signal(signal.SIGTERM, driver.signal_handler)

# Start it!
driver.start()
