#include <mbed-os/targets/TARGET_NXP/TARGET_LPC176X/device/cmsis.h>
#include "MotorDriverManagerRS485.h"

MotorDriverManagerRS485::MotorDriverManagerRS485(PinName txPinName, PinName rxPinName):
 serial(txPinName, rxPinName) {

    receiveCounter = 0;

    speeds[0] = 0;
    speeds[1] = 0;
    speeds[2] = 0;
    speeds[3] = 0;
    speeds[4] = 0;

    actualSpeeds[0] = 0;
    actualSpeeds[1] = 0;
    actualSpeeds[2] = 0;
    actualSpeeds[3] = 0;
    actualSpeeds[4] = 0;

    deviceIds[0] = '1';
    deviceIds[1] = '2';
    deviceIds[2] = '3';
    deviceIds[3] = '4';
    deviceIds[4] = '5';

    activeSpeedIndex = 0;
    isSettingSpeeds = false;
    sendNextSpeed = false;

    txDelayCount = 1;
    txDelayCounter = 0;
    txDelayActive = 0;
    txSend = 0;

    if (rxPinName == P2_1) {
        serialId = 1;
    } else if (rxPinName == P0_11) {
        serialId = 2;
    } else {
        serialId = 0;
    }

    serial.attach(this, &MotorDriverManagerRS485::rxHandler);
}

void MotorDriverManagerRS485::baud(int baudrate) {
    serial.baud(baudrate);
}

void MotorDriverManagerRS485::rxHandler(void) {
    // Interrupt does not work with RTOS when using standard functions (getc, putc)
    // https://developer.mbed.org/forum/bugs-suggestions/topic/4217/

    //while (serial.readable()) {
    while (isSerialReadable()) {
        //char c = device.getc();
        char c = serialReadChar();

        if (receiveCounter < 8) {
            switch (receiveCounter) {
                case 0:
                    if (c == '<') {
                        receiveBuffer[receiveCounter] = c;
                        receiveCounter++;
                    } else {
                        receiveCounter = 0;
                    }
                    break;
                case 1:
                    if (c >= '1' && c <= '5') {
                        receiveBuffer[receiveCounter] = c;
                        receiveCounter++;
                    } else {
                        receiveCounter = 0;
                    }
                    break;
                case 2:
                    if (c == 'd') {
                        receiveBuffer[receiveCounter] = c;
                        receiveCounter++;
                    } else {
                        receiveCounter = 0;
                    }
                    break;
                case 3:
                case 4:
                case 5:
                case 6:
                    receiveBuffer[receiveCounter] = c;
                    receiveCounter++;
                    break;
                case 7:
                    if (c == '>') {
                        receiveBuffer[receiveCounter] = c;
                        receiveCounter++;
                    } else {
                        receiveCounter = 0;
                    }
                    break;
                default:
                    receiveCounter = 0;
            }
        }
    }
}

void MotorDriverManagerRS485::setSpeeds(int speed1, int speed2, int speed3, int speed4, int speed5) {
    speeds[1] = speed1;
    speeds[2] = speed2;
    speeds[0] = speed3;
    speeds[3] = speed4;
    speeds[4] = speed5;

    isSettingSpeeds = true;
    txSend = 1;
}

void MotorDriverManagerRS485::update() {
    if (receiveCounter == 8) {
        if (receiveBuffer[2] == 'd') {
            int value = ((int)receiveBuffer[3]) | ((int)receiveBuffer[4] << 8) | ((int)receiveBuffer[5] << 16) | ((int)receiveBuffer[6] << 24);
            value = ((value >> 8) * 1000) >> 16;

            if (receiveBuffer[1] == deviceIds[activeSpeedIndex]) {
                actualSpeeds[activeSpeedIndex] = value;
                //sendNextSpeed = true;
                //txDelayActive = 1;
                if (activeSpeedIndex == 4) {
                    isSettingSpeeds = false;
                } else {
                    txSend = 1;
                }
            }

            activeSpeedIndex++;

            if (activeSpeedIndex == 5) {
                activeSpeedIndex = 0;

                _callback.call();
            }
        } else {
            txDelayActive = 1;
            txSend = 1;
        }

        receiveCounter = 0;
    }

    if (txSend) {
        txSend = 0;

        if (isSettingSpeeds) {
            //sendNextSpeed = false;
            int qSpeed = ((speeds[activeSpeedIndex] << 16) / 1000) << 8;

            sendBuffer[0] = '<';
            sendBuffer[1] = deviceIds[activeSpeedIndex];
            sendBuffer[2] = 's';

            int * intlocation = (int*)(&sendBuffer[3]);
            *intlocation = qSpeed;

            sendBuffer[7] = '>';

            serialWrite(sendBuffer, 8);
        }
    }
}

void MotorDriverManagerRS485::serialWrite(char *sendData, int length) {
    int i = 0;

    while (i < length) {
        if (serial.writeable()) {
            serial.putc(sendData[i]);
            i++;
        }
    }
}



int *MotorDriverManagerRS485::getSpeeds() {
    return actualSpeeds;
}

bool MotorDriverManagerRS485::isSerialReadable() {
    if (serialId == 1) {
        return LPC_UART1->LSR & (uint8_t)0x01;
    }

    if (serialId == 2) {
        return LPC_UART2->LSR & (uint8_t)0x01;
    }

    return LPC_UART0->LSR & (uint8_t)0x01;
}

char MotorDriverManagerRS485::serialReadChar() {
    if (serialId == 1) {
        return LPC_UART1->RBR;
    }

    if (serialId == 2) {
        return LPC_UART2->RBR;
    }

    return LPC_UART0->RBR;
}
