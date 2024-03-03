import adafruit_dht as dht_sensor
import time
from flask import Flask, Response
from prometheus_client import Counter, Gauge, start_http_server, generate_latest

content_type = str('text/plain; version=0.0.4; charset=utf-8')

def get_temperature_readings():
    humidity, temperature = dht_sensor.read_retry(dht_sensor.DHT22, 4)
    humidity = format(humidity, ".2f")
    temperature = format(temperature, ".2f")
    if all(v is not None for v in [humidity, temperature]):
        response = {"Temperatura": temperature, "Wilgotność": humidity}
        return response
    else:
        time.sleep(0.2)
        humidity, temperature = dht_sensor.read_retry(dht_sensor.DHT22, 4)
        humidity = format(humidity, ".2f")
        temperature = format(temperature, ".2f")
        response = {"Temperatura": temperature, "Wilgotność": humidity}
        return response

app = Flask(__name__)

current_humidity = Gauge(
        'current_humidity',
        'the current humidity percentage, this is a gauge as the value can increase or decrease',
        ['room']
)

current_temperature = Gauge(
        'current_temperature',
        'the current temperature in celsius, this is a gauge as the value can increase or decrease',
        ['room']
)

@app.route('/metrics')
def metrics():
    metrics = get_temperature_readings()
    current_humidity.labels('study').set(metrics['humidity'])
    current_temperature.labels('study').set(metrics['temperature'])
    return Response(generate_latest(), mimetype=content_type)

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
