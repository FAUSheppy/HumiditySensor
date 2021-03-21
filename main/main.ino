#include <SimpleDHT.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <base64.h>
#define ONE_SECOND 1000
#define TEXT_SIZE  2

#ifdef with_display

    /* OLED_RESET-pin to '-1' because reset pin is shared */
    #define OLED_RESET    -1
    #define SCREEN_WIDTH 128
    #define SCREEN_HEIGHT 32

    Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#endif


/* gpio 2 == d4 on esp8266 */
#define pinDHT11 2
SimpleDHT11 dht11(pinDHT11);

/* network and transmission settings */
#include "settings.h"

/* clients */
WiFiClientSecure client;

void setup() {

  Serial.begin(SERIAL_DEFAULT);

  Serial.println(F("WiFI Setup Started"));


  Serial.println(F("WiFI Setup Completed"));

  #ifdef with_display

    /* SSD1306_SWITCHCAPVCC = generate OLED-display voltage from 3.3V internally */
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { /* Address 0x3C for 128x32 */
      Serial.println(F("SSD1306 allocation failed"));
      for(;;);
    }

    display.clearDisplay();
    display.setTextSize(TEXT_SIZE);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.cp437(true);
    display.display();

  #endif

}

void connectToWiFI(){
    if(WiFi.status() == WL_CONNECTED){
        return;
    }else{
        WiFi.disconnect();
        WiFi.begin(WLAN_SSID, WLAN_PASSWORD);
        while (WiFi.status() != WL_CONNECTED){
            Serial.println(F("WiFI Setup waiting"));
            delay(ONE_SECOND);
        }
    }
}

void loop() {

  /* read sensor */
  byte temperature = 0;
  byte humidity    = 0;
  int  err         = SimpleDHTErrSuccess;

  if((err = dht11.read(&temperature, &humidity, NULL)) != SimpleDHTErrSuccess) {
    Serial.print("Read DHT11 failed, err=");
    Serial.println(err);
    delay(10000);
    return;
  }

  #ifdef with_display
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("TEM: ");
    display.println((int) temperature);
    display.setCursor(0,14);
    display.print("HUM: ");
    display.println((int) humidity);
    display.display();
  #endif

  client.setInsecure();
  client.connect(TARGET_INFLUX_SERVER, TARGET_INFLUX_PORT);
  HTTPClient http;
  http.begin(client, TARGET_INFLUX_SERVER);

  String dataTemp = "temperature,location=" + String(SENSOR_LOCATION) +" value=" + String(temperature);
  String dataHum  = "humidity,location="    + String(SENSOR_LOCATION) +" value=" + String(humidity);

  String auth = base64::encode(String(INFLUX_AUTH_USER) + ":" + String(INFLUX_AUTH_PW));
  
  http.addHeader("Authorization", "Basic " + auth);
  auto retTemp = http.POST(dataTemp);
  
  Serial.println(dataTemp);
  Serial.println(TARGET_INFLUX_SERVER);
  Serial.println(TARGET_INFLUX_PORT);
  Serial.println(retTemp);
  
  http.addHeader("Authorization", "Basic " + auth);
  auto retHum  = http.POST(dataHum);

  delay(MESSURMENT_DELAY);
}
