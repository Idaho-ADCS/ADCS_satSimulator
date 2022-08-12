#include "Arduino.h"

//"groundstation" side of CD&H simulator
//MAC address of ESP32:  24:D7:EB:10:EE:34

//MAC address of 2nd ESP32:  24:D7:EB:10:62:14

// Dependencies
#include "esp_now.h"
#include "WiFi.h"

#include "comm.h"

// MODE ----- use to configure transmitter, or satellite side {false=satellite}
bool sender = false;

// Definitions
uint8_t satAddress[] = {0x24, 0xD7, 0xEB, 0x10, 0xEE, 0x34};
uint8_t senderAddress[] = {0x24, 0xD7, 0xEB, 0x10, 0x62, 0x14};

float message1;
int message2;

float sender_message1;
int sender_message2;

String success;

//for serial communication to interface between computer and esp32
String incomingCommand ; //will be read from serial port

//esp-sat periferals -> for setting up enable pin, UART communication
#define GPIO_ENABLE 21


// Structure for sending data
typedef struct struct_message {
    float message1;  //currently unused (timestamp)
    int message2;    //currently used to send a command to esp-sat
}struct_message;

struct_message testaroo;
struct_message sender_readings;

// Functions
void display(){
    Serial.println("Incoming Data:");
    Serial.println(sender_readings.message1);
    Serial.println(sender_readings.message2);
    Serial.println();
}

void enableADCS(){
    digitalWrite(GPIO_ENABLE, HIGH);

    if(sender == false){
        init_uart();
    }

}

void disableADCS(){
    digitalWrite(GPIO_ENABLE, LOW);

    if(sender == false){
        disable_uart();
    }

}
// translate the index into a command to send this is between esp-interface and esp-sat
void sendCommand(int cmdIndex){
  switch(cmdIndex){
    case 0: // standby
      send_command(CMD_STANDBY);
      break;
    case 1: // heartbeat
      send_command(CMD_HEARTBEAT);
      break;
    case 2: // orient
      send_command(CMD_TST_SIMPLE_ORIENT);
      break;
    case 3: // motion
      send_command(CMD_TST_BASIC_MOTION);
      break;
    case 4: //simple detumble
      send_command(CMD_TST_SIMPLE_DETUMBLE);
      break;
    case 5: // motion
      send_command(CMD_TST_BLDC);
      break;
    case 6: // motion
      send_command(CMD_TST_MTX);
      break;  
    case 98: //enable 
      enableADCS();
      break;
    case 99: //disable
      disableADCS();
      break;

    default:
      Serial.println("[ERROR] cmd index selected from webserver is out of bounds");
  }
}

//callback when data is sent
void data_sent(const uint8_t*mac_addr, esp_now_send_status_t status){
    Serial.println("Last Packet Send Status");
    Serial.println(status==ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    if(status==0){
        success="Delivery Success";
    }
    else{
        success="Delivery Fail";
    }
}

//callback when data is recieved
void data_receive(const uint8_t *mac, const uint8_t *incomingData, int len){
    memcpy(&sender_readings, incomingData, len);
    Serial.print("Bytes recieved: ");
    Serial.println(len);
    sender_message1 = sender_readings.message1;
    sender_message2 = sender_readings.message2;
    //display();

    //if this is the esp-sat, then pass the command along to the ADCS
    if(sender==false){
        sendCommand(sender_message2);
    }

}


void setup(){
    Serial.begin(115200);
    
    WiFi.mode(WIFI_STA);
 
    //check to see if esp-NOW protocol initializes correctly
    if(esp_now_init() != ESP_OK){
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    

    //add callback function for sent data
    esp_now_register_send_cb(data_sent);

    //pair esp boards
    esp_now_peer_info_t peerInfo;

    //decide which address to use
    if(sender == true){
        memcpy(peerInfo.peer_addr, satAddress, 6);
    }
    else{
        memcpy(peerInfo.peer_addr, senderAddress, 6);
    }
    
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    if(esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
    }
    

    // add callback function for when data is recieved
    esp_now_register_recv_cb(data_receive);

    //setup peripherals
    pinMode(GPIO_ENABLE, OUTPUT);
}

void loop(){
    //fill messages
    testaroo.message1 = float(millis());
    
    esp_err_t result;

    /*
    if(sender==false){
        sendCommand(4);
        delay(3000);
    }
    */

    //transmit - this is the interface between user and transmitter esp32
    if(sender==true){
        
        //check for serial input
        if(Serial.available()>0){
            incomingCommand = Serial.readString();
            testaroo.message2 = incomingCommand.toInt();
            //pass along to adcs-side esp32
            result = esp_now_send(satAddress, (uint8_t *)&testaroo,sizeof(testaroo));
            delay(2);

            if(result == ESP_OK){
            Serial.println("Sent with success");
            }
            else{
            Serial.println("Error sending message");
            }
        }
    }

}
