import os
import time
import adafruit_dht
import board

dht_device = adafruit_dht.DHT22(board.D4)

try:
    f = open('/home/michu990/dht22/log.csv', 'a+')
    if os.stat('/home/michu990/dht22/log.csv').st_size == 0:
            f.write('Date,Time,Temperature C, Temperature F,Humidity\r\n')
except:
    pass

while True:
    try:
        temperature_c = dht_device.temperature
        temperature_f = temperature_c * (9 / 5) + 32

        humidity = dht_device.humidity
