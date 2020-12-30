#ifndef PERIPHERALIO_H_
#define PERIPHERALIO_H_
#include "stdint.h"

class Limitswitch{
    public:
        void init(uint8_t pinNum);
        bool read(uint8_t pinNum);
};
#endif