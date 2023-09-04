#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// Replace with your network credentials
const char* ssid = "name";
const char* password = "password";

// REPLACE with your Domain name and URL path or IP address with path
const char* serverName = "http://localhost/gotas/post-data.php";

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 
String apiKeyValue = "tPmAT5Ab3j7F9";

#define sensorPin 32
#define signalPin 33
int sensorValue = 0;
int lastSensorValue = 0;
int difference = 50;

int readSensor()
{
  sensorValue = analogRead(sensorPin);
  Serial.println(sensorValue);

  if(sensorValue <= lastSensorValue - difference){
    digitalWrite(signalPin, HIGH);
    lastSensorValue = sensorValue;
    return sensorValue;
    //delay(500);
  }
  else{
    digitalWrite(signalPin, LOW);
    lastSensorValue = sensorValue;
    return -1;
  }
}

void setup() {
  Serial.begin(115200);

  pinMode(sensorPin, INPUT);
  pinMode(signalPin, OUTPUT);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Reading sensor
  int sensorValue = readSensor();

  //Check WiFi connection status
  if(WiFi.status()== WL_CONNECTED && sensorValue != -1){
    WiFiClient *client = new WiFiClient;
    //client->setInsecure(); //don't use SSL certificate
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(serverName);

    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&sensor_value=" + String(sensorValue) + "";
    // POST
    int httpResponseCode = http.POST(httpRequestData);
    
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("ERROR: ");
      Serial.println(http.errorToString(httpResponseCode));
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }

  delay(0.5);  
}
