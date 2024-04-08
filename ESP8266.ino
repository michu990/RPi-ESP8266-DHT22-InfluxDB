#include "DHT.h"
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <InfluxDb.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>

#define DHTPIN 12
#define DHTTYPE DHT22
#define INFLUXDB_URL ""
#define WIFI_SSID ""
#define WIFI_PASS ""
#define INFLUXDB_TOKEN ""
#define INFLUXDB_ORG ""
#define INFLUXDB_BUCKET ""

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);

ESP8266WiFiMulti WiFiMulti;
//Influxdb influx(INFLUXDB_URL);

DHT dht(DHTPIN, DHTTYPE);
Point sensor("DHT22");

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(1000);
  WiFiMulti.addAP(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WIFI");
  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
    }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
//  influx.setDb("DHT22");
  if (client.validateConnection()) {
      Serial.print("Connected to InfluxDB: ");
      Serial.println(client.getServerUrl());
    } else {
      Serial.print("InfluxDB connection failed: ");
      Serial.println(client.getLastErrorMessage());
    }

   // sensor.addTag("device", DEVICE);
   // sensor.addTag("SSID", WiFi.SSID());

  while(!Serial) { }
  
  dht.begin();

  Serial.println("Device Started");
  Serial.println("-------------------------------------");
  Serial.println("Running DHT!");
  Serial.println("-------------------------------------");

}
void loop() {
    float humid = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float temperatureC = dht.readTemperature();
    // Read temperature as Fahrenheit (isFahrenheit = true)
    float tempf = dht.readTemperature(true);

  Serial.print(temperatureC);
  Serial.println(" ºC");
  //Serial.print(F);
  //Serial.println(" ºF");
  Serial.print(humid);
  Serial.println(" %");

  InfluxData sensor("DHT22");
  //sensor.addTag("Device", "ESP8266");
  //sensor.addTag("Sensor", "Temp");
  //sensor.addTag("Unit", "C");
  //sensor.addTag("Sensor2", "Hum");
  //sensor.addTag("Unit", "H");
  
  sensor.addField("temperatureC", temperatureC);
  sensor.addField("humidity", humid);
  sensor.addField("temperatureF", tempf);
  //You can change the data being sent to influx either ºC or ºF here
  //row.addField("Temp", tempf);
  //row.addField("Hum", humid);

  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  
//  sensor.add(sensor);
  delay(1000);
}
