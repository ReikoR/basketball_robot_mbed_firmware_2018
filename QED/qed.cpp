#include "qed.h"

QED::QED(PinName channelA, PinName channelB)
        : interruptA(channelA), interruptB(channelB) {

    pulses = 0;
    currState = (interruptA.read() << 1) | (interruptB.read());
    prevState = currState;

    interruptA.rise(this, &QED::decode);
    interruptA.fall(this, &QED::decode);
    interruptB.rise(this, &QED::decode);
    interruptB.fall(this, &QED::decode);
}

int QED::read() {
    int pulseCount = pulses;
    pulses = 0;
    return pulseCount;
}

void QED::decode() {
    currState = (interruptA.read() << 1) | (interruptB.read());

    int change = (prevState & PREV_MASK) ^ ((currState & CURR_MASK) >> 1);

    if (change == 0) {
        change = -1;
    }

    pulses -= change;
    prevState = currState;
}