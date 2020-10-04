/*
 * Rui Santos 
 * Complete Project Details https://randomnerdtutorials.com
 */
 
#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

#include <Wire.h>


// Replace with your SSID and Password
const char* ssid     = "Kamensky";
const char* password = "12345678";

// Replace with your unique IFTTT URL resource
const char* resource = "/trigger/newDataReceived/with/key/dUMQdhnqWMG9SynemYKAAH";

// How your resource variable should look like, but with your own API KEY (that API KEY below is just an example):
//const char* resource = "/trigger/bme280_readings/with/key/nAZjOphL3d-ZO4N3k64-1A7gTlNSrxMJdmqy3";

// Maker Webhooks IFTTT
const char* server = "maker.ifttt.com";

// Time to sleep
uint64_t uS_TO_S_FACTOR = 1000000;  // Conversion factor for micro seconds to seconds
// sleep for 30 minutes = 1800 seconds
uint64_t TIME_TO_SLEEP = 5;

boolean dataSend = false;

String c;

String dataToSend = "";

String temp1 = "";
String temp2 = "";
String speedValue = "";
String dose = "";
String threshold = "";
String injection = "";
String fill = "";
String holder = "";
String error = "";




void setup() {
  Serial.begin(19200); 
  delay(2000);

  initWifi();

    
//  #ifdef ESP32
//    // enable timer deep sleep
//    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);    
//    Serial.println("Going to sleep now");
//    // start deep sleep for 3600 seconds (60 minutes)
//    esp_deep_sleep_start();
//  #else
//    // Deep sleep mode for 3600 seconds (60 minutes)
//    Serial.println("Going to sleep now");
//    ESP.deepSleep(TIME_TO_SLEEP * uS_TO_S_FACTOR); 
//  #endif
}

void loop() {
  if(Serial.available()>0){
  dataToSend = "";
  c = Serial.readStringUntil('\n');
  int beginningOfMessage = c.indexOf("temp1");
  c.remove(0,beginningOfMessage);
  Serial.println(c.length());
// if(c.length() == 106){
temp1 = c.substring(6,11);
temp2 = c.substring(17,22);
speedValue = c.substring(28,32);
dose = c.substring(37,41);
threshold = c.substring(51,55);
injection = c.substring(65,69);
fill = c.substring(74,78);
holder = c.substring(85,91);
error = c.substring(97,105);

dataToSend = temp1;
dataToSend += "|||";
dataToSend += temp2;
dataToSend += "|||";
dataToSend += speedValue;
dataToSend += "|||";
dataToSend += dose;
dataToSend += "|||";
dataToSend += threshold;
dataToSend += "|||";
dataToSend += injection;
dataToSend += "|||";
dataToSend += fill;
dataToSend += "|||";
dataToSend += holder;
dataToSend += "|||";
dataToSend += error;
Serial.println(dataToSend);
 // }
//  Serial.println(c.length());
  makeIFTTTRequest(); 
  }
}

// Establish a Wi-Fi connection with your router
void initWifi() {
//  Serial.print("Connecting to: "); 
//  Serial.print(ssid);
  WiFi.begin(ssid, password);  

//  int timeout = 10 * 4; // 10 seconds
  while(WiFi.status() != WL_CONNECTED) {
    delay(250);
//    Serial.print(".");
  }
//  Serial.println("");

  if(WiFi.status() != WL_CONNECTED) {
     Serial.println("Failed to connect, going back to sleep");
  }

//  Serial.print("WiFi connected in: "); 
//  Serial.print(millis());
//  Serial.print(", IP address: "); 
//  Serial.println(WiFi.localIP());
}

// Make an HTTP request to the IFTTT web service
void makeIFTTTRequest() {
  Serial.print("Connecting to "); 
  Serial.print(server);
  
  WiFiClient client;
  int retries = 5;
  while(!!!client.connect(server, 80) && (retries-- > 0)) {
    Serial.print(".");
  }
  Serial.println();
  if(!!!client.connected()) {
    Serial.println("Failed to connect...");
  }
  
  Serial.print("Request resource: "); 
  Serial.println(resource);
  Serial.println(dataToSend);
  // Temperature in Celsius
  String jsonObject = String("{\"value1\":\"") + dataToSend + "\"}";
                      
  // Comment the previous line and uncomment the next line to publish temperature readings in Fahrenheit                    
  /*String jsonObject = String("{\"value1\":\"") + (1.8 * bme.readTemperature() + 32) + "\",\"value2\":\"" 
                      + (bme.readPressure()/100.0F) + "\",\"value3\":\"" + bme.readHumidity() + "\"}";*/
                      
  client.println(String("POST ") + resource + " HTTP/1.1");
  client.println(String("Host: ") + server); 
  client.println("Connection: close\r\nContent-Type: application/json");
  client.print("Content-Length: ");
  client.println(jsonObject.length());
  client.println();
  client.println(jsonObject);
        
  int timeout = 5 * 10; // 5 seconds             
  while(!!!client.available() && (timeout-- > 0)){
    delay(100);
  }
  if(!!!client.available()) {
    Serial.println("No response...");
  }
  while(client.available()){
    Serial.write(client.read());
  }
  
  Serial.println("\nclosing connection");
  client.stop();
}
