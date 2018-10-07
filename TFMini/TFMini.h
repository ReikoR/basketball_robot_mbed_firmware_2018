#ifndef MBED_TEST_TFMINI_H
#define MBED_TEST_TFMINI_H

#include "mbed.h"

class TFMini {
protected:
    FunctionPointer _callback;

public:
    TFMini(PinName txPinName, PinName rxPinName);

    struct Frame {
        uint16_t distance;
        uint16_t strength;
        uint8_t distanceMode;
        uint8_t checkSum;
    };

    void baud(int baudrate);

    Frame *read();

    bool readable();

    void attach(void (*function)(void)) {
        _callback.attach(function);
    }

    template<typename T>
    void attach(T *object, void (T::*member)(void)) {
        _callback.attach( object, member );
    }

private:
    Serial serial;

    Frame frame;

    int serialId;

    bool isSerialReadable();

    void rxHandler(void);

    bool messageAvailable;

    char serialReadChar();

    unsigned int receiveCounter;
    uint8_t receiveBuffer[16];

    uint8_t receivedMessage[16];

    int commandLength;
};

#endif //MBED_TEST_TFMINI_H
