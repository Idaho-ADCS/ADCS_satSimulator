/*
* This script is useful for sending single commands to adcs via UART
*/

#include  "Arduino.h"

#include "comm.h"



void setup(){
    init_uart();
    Serial.begin(9600);


}

void loop(){
    Serial.println("tic");
    //init_uart();
    //delay(10);
    send_command(CMD_TST_SIMPLE_DETUMBLE);
    //disable_uart();
    //Serial.println("disabled uart");
    delay(3000);
    //init_uart();
    //delay(10);
    //send_command(CMD_TST_BASIC_MOTION);
    //disable_uart();
    //delay(10000);
    Serial.println("toc");
}