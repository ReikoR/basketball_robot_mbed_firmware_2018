#include <mbed-os/targets/TARGET_NXP/TARGET_LPC176X/device/cmsis.h>
#include "CisecoManager.h"

CisecoManager::CisecoManager(PinName txPinName, PinName rxPinName):
        serial(txPinName, rxPinName), buf(64) {

    messageAvailable = false;
    receiveCounter = 0;
    shortCommandsEnabled = false;
    shortCommandLength = 5;
    longCommandLength = 12;
    commandLength = longCommandLength;

    if (rxPinName == P2_1) {
        serialId = 1;
    } else if (rxPinName == P0_11) {
        serialId = 2;
    } else {
        serialId = 0;
    }

    serial.attach(this, &CisecoManager::rxHandler);
}

void CisecoManager::baud(int baudrate) {
    serial.baud(baudrate);
}

void CisecoManager::rxHandler(void) {
    // Interrupt does not work with RTOS when using standard functions (getc, putc)
    // https://developer.mbed.org/forum/bugs-suggestions/topic/4217/

    while (serial.readable()) {
        char c = serialReadChar();

        if (receiveCounter < commandLength) {
            if (receiveCounter == 0) {
                // Do not continue before a is received
                if (c == 'a') {
                    receiveBuffer[receiveCounter] = c;
                    receiveCounter++;
                }
            } else if (c == 'a' && !shortCommandsEnabled
                || c == 'a' && shortCommandsEnabled && (receiveCounter < commandLength - 1)
            ) {
                // If a is received in the middle, assume some bytes got lost before and start from beginning
                receiveCounter = 0;

                receiveBuffer[receiveCounter] = c;
                receiveCounter++;
            } else {
                receiveBuffer[receiveCounter] = c;
                receiveCounter++;
            }

            if (receiveCounter == commandLength) {
                receiveCounter = 0;

                for (unsigned int i = 0; i < commandLength; i++) {
                    buf.queue(receiveBuffer[i]);
                }

                if (!messageAvailable) {
                    handleMessage();
                    //break;
                }
            }
        }
    }
}

bool CisecoManager::readable() {
    return messageAvailable;
}

char *CisecoManager::read() {
    messageAvailable = false;
    return receivedMessage;
}

void CisecoManager::send(char *sendData) {
    serialWrite(sendData, commandLength);
}

void CisecoManager::send(char *sendData, int length) {
    serialWrite(sendData, length);
}

void CisecoManager::update() {
    /*if (receiveCounter == commandLength) {
        handleMessage();
        _callback.call();
    }*/

    if (buf.available() >= commandLength) {
        handleMessage();
    }
}

void CisecoManager::handleMessage() {
    if (messageAvailable) {
        return;
    }

    for (unsigned int i = 0; i < commandLength; i++) {
        buf.dequeue(receivedMessage + i);
    }

    receivedMessage[commandLength] = '\0';

    /*receiveCounter = 0;

    memcpy(receivedMessage, receiveBuffer, sizeof(receiveBuffer));*/

    messageAvailable = true;
}

void CisecoManager::serialWrite(char *sendData, int length) {
    int i = 0;

    while (i < length) {
        if (serial.writeable()) {
            serial.putc(sendData[i]);
        }
        i++;
    }
}

char CisecoManager::serialReadChar() {
    if (serialId == 1) {
        return LPC_UART1->RBR;
    }

    if (serialId == 2) {
        return LPC_UART2->RBR;
    }

    return LPC_UART0->RBR;
}

void CisecoManager::setShortCommandMode(bool isEnabled) {
    shortCommandsEnabled = isEnabled;
    receiveCounter = 0;

    if (isEnabled) {
        commandLength = shortCommandLength;
    } else {
        commandLength = longCommandLength;
    }
}
