#include <Arduino.h>
#include "comms.h"

void Communications::init(int32_t baud){
    Serial.begin(baud);
}

void Communications::checkCommand(void){
    String incomingString;
    if(Serial.available() > 0){
        commandList(Serial.readStringUntil('\n'));
        commandisTrue = true;
    }
    else{
        commandisTrue = false;
    }
}

bool Communications::sendMessage(void){

}

bool Communications::messageList(void){

}

bool Communications::commandList(String input){
    input.toLowerCase();
    if(input == "move"){
        Serial.print("Enter Steps, ");
        Serial.flush();
        while(Serial.available() == 0){
            arg1 = Serial.parseInt();
        }
        inputCommands =  SetPosition;
        return true;
    }
    else if (input == "tare"){
        Serial.println("tare, ");
        inputCommands = Tare;
        return true;
    }
    else if(input == "get weight"){
        Serial.println("get weight, ");
        inputCommands = GetWeight;
        return true;
    }
    else if(input == "measure tension"){
        Serial.println("measure tension, ");
        inputCommands = MeasureTension;
        return true;
    }
    else if(input == "set position"){
        Serial.println("set Position, ");
        inputCommands = SetPosition;
        return true;
    }
    else if(input == "home"){
        Serial.println("Home, ");
        inputCommands = Home;
        return true;
    }
    else if(input == "position"){
        Serial.println("position,");
        inputCommands = GetPosition;
        return true;
    }
    else{
        Serial.println("Invalid Arguments, ");
        inputCommands = InvalidArgs;
        return false;
    }

}