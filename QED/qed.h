#ifndef BBR18_RK_MBED_FIRMWARE_QED_H
#define BBR18_RK_MBED_FIRMWARE_QED_H

#include "mbed.h"

#define PREV_MASK 0x1 //Mask for the previous state in determining direction of rotation.
#define CURR_MASK 0x2 //Mask for the current state in determining direction of rotation.
#define INVALID   0x3 //XORing two states where both bits have changed.

class QED {
public:
    QED(PinName channelA, PinName channelB);

    int read();

private:
    void decode();

    InterruptIn interruptA;
    InterruptIn interruptB;

    volatile int pulses;

    int prevState;
    int currState;
};

#endif //BBR18_RK_MBED_FIRMWARE_QED_H
