#include <Arduino.h>
#include "comms.h"
#include "config.h"


void Communications::init(void){
    Serial.begin(BAUD_RATE);
}

bool Communications::checkCommand(void){
    String incomingString;
    if(Serial.available() > 0){
        commandList(Serial.readStringUntil('\n'));
        return true;
    }
    else{
        return false;
    }
}

bool Communications::sendMessage(void){

}

bool Communications::messageList(void){

}

commands Communications::commandList(String input){
    input.toLowerCase();
    if(input == "up"){
        Serial.println("Enter Steps");
        while(Serial.available() == 0){
            inputValue = Serial.parseInt();
        }
        return MoveUp;
    }
    else if (input == "down"){
        Serial.println("Enter Steps");
        while(Serial.available() == 0){
            inputValue = Serial.parseInt();
        }
        return MoveDown;
    }
    else if (input == "tare"){
        return Tare;
    }
    else{
        Serial.println("Invalid Arguments");
        return InvalidArgs;
    }

}