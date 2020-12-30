#ifndef COMMS_H_
#define COMMS_H_

#include "stdint.h"
enum commands{
    InvalidArgs,
    Tare,
    GetWeight,
    MeasureTension,
    SetPosition,
    SetTension,
    Home,
    GetPosition,
    NoCommand
};

class Communications{
    public:
        int32_t arg1 = 0;
        commands inputCommands = NoCommand;
        bool commandisTrue = false;
    public:
        void init(int32_t baud);
        void checkCommand(void);
        bool sendMessage(void);
    private:
        bool messageList(void);
        bool commandList(String input);
};
#endif