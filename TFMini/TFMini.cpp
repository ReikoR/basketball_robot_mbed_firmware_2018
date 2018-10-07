#include <mbed-os/targets/TARGET_NXP/TARGET_LPC176X/device/cmsis.h>
#include "TFMini.h"

TFMini::TFMini(PinName txPinName, PinName rxPinName):
        serial(txPinName, rxPinName) {

    messageAvailable = false;
    receiveCounter = 0;
    commandLength = 9;

    if (rxPinName == P2_1) {
        serialId = 1;
    } else if (rxPinName == P0_11) {
        serialId = 2;
    } else if (rxPinName == P0_1) {
        serialId = 3;
    } else {
        serialId = 0;
    }

    serial.attach(this, &TFMini::rxHandler);
}

void TFMini::baud(int baudrate) {
    serial.baud(baudrate);
}

void TFMini::rxHandler(void) {
    // Interrupt does not work with RTOS when using standard functions (getc, putc)
    // https://developer.mbed.org/forum/bugs-suggestions/topic/4217/

    //while (serial.readable()) {
    while (isSerialReadable()) {
        char c = serialReadChar();

        if (receiveCounter < commandLength) {
            if (receiveCounter == 0 || receiveCounter == 1) {
                // Do not continue before 0x59 is received
                if (c == 0x59) {
                    receiveBuffer[receiveCounter] = c;
                    receiveCounter++;
                } else {
                    receiveCounter = 0;
                }
            } else {
                receiveBuffer[receiveCounter] = c;
                receiveCounter++;
            }

            if (receiveCounter == commandLength) {
                receiveCounter = 0;

                for (unsigned int i = 0; i < commandLength; i++) {
                    receivedMessage[i] = receiveBuffer[i];
                }

                frame.distance = (uint16_t)receivedMessage[3] << 8 | (uint16_t)receivedMessage[2];
                frame.strength = (uint16_t)receivedMessage[5] << 8 | (uint16_t)receivedMessage[4];
                frame.distanceMode = receivedMessage[6];
                frame.checkSum = receivedMessage[8];

                messageAvailable = true;
            }
        }
    }
}

bool TFMini::readable() {
    return messageAvailable;
}

bool TFMini::isSerialReadable() {
    if (serialId == 1) {
        return LPC_UART1->LSR & (uint8_t)0x01;
    }

    if (serialId == 2) {
        return LPC_UART2->LSR & (uint8_t)0x01;
    }

    if (serialId == 3) {
        return LPC_UART3->LSR & (uint8_t)0x01;
    }

    return LPC_UART0->LSR & (uint8_t)0x01;
}

TFMini::Frame *TFMini::read() {
    messageAvailable = false;
    return &frame;
}

char TFMini::serialReadChar() {
    if (serialId == 1) {
        return LPC_UART1->RBR;
    }

    if (serialId == 2) {
        return LPC_UART2->RBR;
    }

    if (serialId == 3) {
        return LPC_UART3->RBR;
    }

    return LPC_UART0->RBR;
}