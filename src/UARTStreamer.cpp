#include "Arduino.h"
#include "HardwareSerial.h"

HardwareSerial testSerial(2);

void setup(){
    Serial.begin(9600);
    testSerial.begin(115200, SERIAL_8O1, 16,17);
}

void loop(){
    if(testSerial.available()){
        int len = testSerial.available();
        //uint8_t incoming = testSerial.read();
        //Serial.println(incoming, HEX);
        
        uint8_t data[len];
        uint8_t incoming2 = testSerial.readBytes(data, len);
        Serial.println(data[0], HEX);
        
        
    }
    delay(10);
}