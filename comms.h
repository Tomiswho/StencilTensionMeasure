#ifndef COMMS_H_
#define COMMS_H_

class Communications{
    public:
        enum commands{
            InvalidArgs,
            MoveUp,
            MoveDown,
            Tare
        };
        int32_t inputValue = 0;
    public:
        void init(void);
        bool checkCommand(void);
        bool sendMessage(void);
    private:
        bool messageList(void);
        commands commandList(String input);
};
#endif