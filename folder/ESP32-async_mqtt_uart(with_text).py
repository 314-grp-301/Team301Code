from mqtt_async import MQTTClient, config
import uasyncio as asyncio
import time
from machine import UART, Pin, SoftI2C
import ssd1306
import logging

# Basic logging setup
logging.basicConfig(level=logging.DEBUG)

# MQTT and UART configuration constants
MAXTX = 4
TOPIC_PUB = 'EGR314/Team301/XT'
TOPIC_SUB = 'EGR314/Team301/XT'
config.server = 'egr3x4.ddns.net'  # MQTT broker address
config.ssid = 'Meyerhoff-AZ'       # WiFi SSID
config.wifi_pw = '9257846050'      # WiFi password

# OLED display setup
i2c = SoftI2C(scl=Pin(22), sda=Pin(21))
oled_width = 128
oled_height = 64
oled = ssd1306.SSD1306_I2C(oled_width, oled_height, i2c)

def print_data(msg):
    """Display a message on the OLED."""
    oled.fill(0)  # Clear the display
    if isinstance(msg, bytes):
        msg = msg.decode('utf-8')  # Decode bytes to string if necessary
    lines = msg.split('\n')
    for i, line in enumerate(lines):
        oled.text(line, 0, i * 10)  # Display each line, spaced 10 pixels apart vertically
    oled.show()

# Setup UART
uart = UART(2, 9600, tx=17, rx=16)
uart.init(9600, bits=8, parity=None, stop=1, flow=0)  # Initialize UART with specified parameters

async def receiver():
    """Asynchronous coroutine to receive data from UART and publish via MQTT."""
    b = b''
    sreader = asyncio.StreamReader(uart)
    while True:
        res = await sreader.read(1)
        if res == b'\r':
            logging.debug(f'Publishing: {b}')
            await client.publish(TOPIC_PUB, b, qos=1)
            b = b''
        else:
            b += res

def callback(topic, msg, retained, qos):
    """Callback function to handle incoming MQTT messages."""
    logging.info(f'Callback received - Topic: {topic}, Message: {msg}, Retained: {retained}, QOS: {qos}')
    print_data(msg)  # Display the received message on the OLED

    while msg:
        uart.write(msg[:MAXTX])
        time.sleep(.01)
        msg = msg[MAXTX:]
    uart.write('\r\n')
    time.sleep(.01)

async def conn_callback(client):
    """Coroutine to handle MQTT connection callback."""
    await client.subscribe(TOPIC_SUB, 1)

async def main(client):
    """Main coroutine to manage MQTT connection and tasks."""
    await client.connect()
    asyncio.create_task(receiver())
    while True:
        await asyncio.sleep(1)

config.subs_cb = callback
config.connect_coro = conn_callback

client = MQTTClient(config)
loop = asyncio.get_event_loop()
loop.run_until_complete(main(client))
