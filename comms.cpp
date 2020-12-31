#include <Arduino.h>
#include "comms.h"

void Communications::init(int32_t baud){
    Serial.begin(baud);
}

void Communications::checkCommand(void){
    String incomingString;
    if(Serial.available() > 0){
        if(commandList(Serial.readStringUntil('\n'))){
          Serial.flush();
        }
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
        while(1){
            if(Serial.available() > 0){
              arg1 = Serial.parseInt();
              break;
            }
        }
        Serial.println(arg1);
        Serial.readStringUntil('\n');
        Serial.flush();
        commandisTrue = true;
        inputCommands =  SetPosition;
        return true;
    }
    else if (input == "tare"){
        Serial.println("tare, ");
        commandisTrue = true;
        inputCommands = Tare;
        return true;
    }
    else if(input == "get weight"){
        Serial.println("get weight, ");
        commandisTrue = true;
        inputCommands = GetWeight;
        return true;
    }
    else if(input == "run"){
        Serial.println("Run, ");
        commandisTrue = true;
        inputCommands = MeasureTension;
        return true;
    }
    else if(input == "set position"){
        Serial.println("set Position, ");
        commandisTrue = true;
        inputCommands = SetPosition;
        return true;
    }
    else if(input == "home"){
        Serial.println("Home, ");
        commandisTrue = true;
        inputCommands = Home;
        return true;
    }
    else if(input == "position"){
        Serial.println("position,");
        commandisTrue = true;
        inputCommands = GetPosition;
        return true;
    }
    else{
        Serial.println("Invalid Arguments");
        commandisTrue = false;
        inputCommands = InvalidArgs;
        return false;
    }

}
