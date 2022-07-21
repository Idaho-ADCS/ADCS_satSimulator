#include <Arduino.h>
//this script finds mac address of esp32

#include "Wifi.h"

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_MODE_STA);
  Serial.println(WiFi.macAddress());

}

void loop() {
  
}