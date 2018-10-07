#ifndef MOTORDRIVERMANAGERRS485_H
#define MOTORDRIVERMANAGERRS485_H

#include "mbed.h"

class MotorDriverManagerRS485 {
protected:
    FunctionPointer _callback;

public:
    MotorDriverManagerRS485(PinName txPinName, PinName rxPinName);

    void baud(int baudrate);

    void setSpeeds(int speed1, int speed2, int speed3, int speed4, int speed5);

    int* getSpeeds();

    void update();

    void attach(void (*function)(void)) {
        _callback.attach(function);
    }

    template<typename T>
    void attach(T *object, void (T::*member)(void)) {
        _callback.attach( object, member );
    }

private:
    Serial serial;

    int serialId;

    void rxHandler(void);

    void serialWrite(char *sendData, int length);
    char serialReadChar();
    bool isSerialReadable();

    int receiveCounter;
    char receiveBuffer[64];
    char sendBuffer[64];

    int speeds[5];
    int actualSpeeds[5];
    char deviceIds[5];
    int activeSpeedIndex;
    bool isSettingSpeeds;
    bool sendNextSpeed;

    int txDelayCount;
    int txDelayCounter;
    int txDelayActive;
    int txSend;
};

#endif //MOTORDRIVERMANAGERRS485_H
