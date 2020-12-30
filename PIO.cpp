#include <Arduino.h>
#include "PIO.h"

void Limitswitch::init(uint8_t pinNum){
    pinMode(pinNum, INPUT);
}

bool Limitswitch::read(uint8_t pinNum){
    return digitalRead(pinNum);
}