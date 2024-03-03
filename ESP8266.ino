#if defined(ESP32)
  #include <WiFiMulti.h>
  WiFiMulti wifiMulti;
  #define DEVICE "ESP32"
  #elif defined(ESP8266)
  #include <ESP8266WiFiMulti.h>
  ESP8266WiFiMulti wifiMulti;
  #define DEVICE "ESP8266"
  #endif
  
  #include <InfluxDbClient.h>
  #include <InfluxDbCloud.h>
  
  // WiFi AP SSID
  #define WIFI_SSID ""
  // WiFi password
  #define WIFI_PASSWORD ""
  
  #define INFLUXDB_URL ""
  #define INFLUXDB_TOKEN ""
  #define INFLUXDB_ORG ""
  #define INFLUXDB_BUCKET ""
  
  // Time zone info
  #define TZ_INFO "UTC1"
  
  // Declare InfluxDB client instance with preconfigured InfluxCloud certificate
  InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
  
  // Declare Data point
  Point sensor("wifi_status");
  
  void setup()
  {
    Serial.begin(115200);
  
    // Setup wifi
    WiFi.mode(WIFI_STA);
    wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  
    Serial.print("Connecting to wifi");
    while (wifiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      delay(100);

    // ... code in setup() from Initialize Client
   
    // Add tags to the data point
    sensor.addTag("device", DEVICE);
    sensor.addTag("SSID", WiFi.SSID());
    }
    Serial.println();
  
    // Accurate time is necessary for certificate validation and writing in batches
    // We use the NTP servers in your area as provided by: https://www.pool.ntp.org/zone/
    // Syncing progress and the time will be printed to Serial.
    timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  
  
    // Check server connection
    if (client.validateConnection()) {
      Serial.print("Connected to InfluxDB: ");
      Serial.println(client.getServerUrl());
    } else {
      Serial.print("InfluxDB connection failed: ");
      Serial.println(client.getLastErrorMessage());
    }
  }

   void loop()
   {
    // Clear fields for reusing the point. Tags will remain the same as set above.
    sensor.clearFields();
  
    // Store measured value into point
    // Report RSSI of currently connected network
    sensor.addField("rssi", WiFi.RSSI());
  
    // Print what are we exactly writing
    Serial.print("Writing: ");
    Serial.println(sensor.toLineProtocol());
  
    // Check WiFi connection and reconnect if needed
    if (wifiMulti.run() != WL_CONNECTED) {
      Serial.println("Wifi connection lost");
    }
  
    // Write point
    if (!client.writePoint(sensor)) {
      Serial.print("InfluxDB write failed: ");
      Serial.println(client.getLastErrorMessage());
    }
  
    Serial.println("Waiting 1 second");
    delay(1000);

 // ... code from Write Data step 
    
    // Query will find the RSSI values for last minute for each connected WiFi network with this device
     String query = "from(bucket: \"DHT22\")\n\
   |> range(start: -1m)\n\
   |> filter(fn: (r) => r._measurement == \"wifi_status\" and r._field == \"rssi\")";
   
     // Print composed query
     Serial.println("Querying for RSSI values written to the \"DHT22\" bucket in the last 1 min... ");
     Serial.println(query);
   
     // Send query to the server and get result
     FluxQueryResult result = client.query(query);
   
     Serial.println("Results : ");
     // Iterate over rows.
     while (result.next()) {
       // Get converted value for flux result column 'SSID'
       String ssid = result.getValueByName("SSID").getString();
       Serial.print("SSID '");
       Serial.print(ssid);
   
       Serial.print("' with RSSI ");
       // Get value of column named '_value'
       long value = result.getValueByName("_value").getLong();
       Serial.print(value);
   
       // Get value for the _time column
       FluxDateTime time = result.getValueByName("_time").getDateTime();
   
       String timeStr = time.format("%F %T");
   
       Serial.print(" at ");
       Serial.print(timeStr);
   
       Serial.println();
     }
   
     // Report any error
     if (result.getError() != "") {
       Serial.print("Query result error: ");
       Serial.println(result.getError());
     }
   
     // Close the result
     result.close();
   
     Serial.println("==========");
   
     delay(5000);

     // ... code from Write Data step
    
    // Query will find the min RSSI value for last minute for each connected WiFi network with this device
      String aggregate_query = "from(bucket: \"DHT22\")\n\
    |> range(start: -1m)\n\
    |> filter(fn: (r) => r._measurement == \"wifi_status\")\n\
    |> min()";
    
      // Print composed query
      Serial.println("Querying for the mean RSSI value written to the \"DHT22\" bucket in the last 1 min... ");
      Serial.println(aggregate_query);
    
      // Send query to the server and get result
      FluxQueryResult aggregate_result = client.query(aggregate_query);
    
      Serial.println("Result : ");
      // Iterate over rows.
      while (aggregate_result.next()) {
        // Get converted value for flux result column 'SSID'
        String ssid = aggregate_result.getValueByName("SSID").getString();
        Serial.print("SSID '");
        Serial.print(ssid);
    
        Serial.print("' with RSSI ");
        // Get value of column named '_value'
        long value = aggregate_result.getValueByName("_value").getLong();
        Serial.print(value);
    
        // Get value for the _time column
        FluxDateTime time = aggregate_result.getValueByName("_time").getDateTime();
    
        String timeStr = time.format("%F %T");
    
        Serial.print(" at ");
        Serial.print(timeStr);
    
        Serial.println();
      }
    
      // Report any error
      if (aggregate_result.getError() != "") {
        Serial.print("Query result error: ");
        Serial.println(aggregate_result.getError());
      }
    
      // Close the result
      aggregate_result.close();
    
      Serial.println("==========");
    
      delay(5000);
    }
