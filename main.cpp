#include "mbed.h"
#include "EthernetInterface.h"
#include "commands.h"
#include "MotorDriverManagerRS485.h"
#include "TFMini.h"

#define PORT 8042
#define MBED_IP_ADDRESS "192.168.4.1"
#define PC_IP_ADDRESS "192.168.4.8"

EthernetInterface eth;
UDPSocket socket;

extern "C" void mbed_mac_address(char *s) {
    char mac[6];
    mac[0] = 0x00;
    mac[1] = 0x02;
    mac[2] = 0xf7;
    mac[3] = 0xf0;
    mac[4] = 0x45;
    mac[5] = 0xbe;
    // Write your own mac address here
    memcpy(s, mac, 6);
}



MotorDriverManagerRS485 motors(P2_0, P2_1);
TFMini tfMini(P0_0, P0_1);

DigitalIn ball1(P2_12);
DigitalIn ball2(P2_13);

Ticker heartbeatTicker;

DigitalOut led1(P0_18);
DigitalOut led2(P0_19);
DigitalOut led3(P0_20);

char recvBuffer[64];
char ethSendBuffer[64];

us_timestamp_t heartBeatPeriod_us = 1000;
bool isHeartbeatUpdate = false;
bool updateLeds = false;

bool returnSpeeds = true;

bool failSafeEnabled = true;
int failSafeCountMotors = 0;
int failSafeLimitMotors = 500;

int ledCount = 1000;
int ledCounter = 0;

int ball1State = 0;
int ball2State = 0;

uint8_t isSpeedChanged = 0;
Timer runningTime;

void heartbeatTick() {
    isHeartbeatUpdate = true;

    if (ledCounter++ > ledCount) {
        ledCounter = 0;

        updateLeds = true;
    }

    isHeartbeatUpdate = true;
}

void sendFeedback() {
    int* speeds = motors.getSpeeds();

    Feedback feedback{};
    feedback.speed1 = static_cast<int16_t>(speeds[0]);
    feedback.speed2 = static_cast<int16_t>(speeds[1]);
    feedback.speed3 = static_cast<int16_t>(speeds[2]);
    feedback.speed4 = static_cast<int16_t>(speeds[3]);
    feedback.speed5 = static_cast<int16_t>(speeds[4]);
    feedback.ball1 = static_cast<uint8_t>(ball1);
    feedback.ball2 = static_cast<uint8_t>(ball2);
    feedback.distance = tfMini.read()->distance;
    feedback.isSpeedChanged = isSpeedChanged;
    feedback.time = runningTime.read_us();

    isSpeedChanged = 0;

    socket.sendto(PC_IP_ADDRESS, PORT, &feedback, sizeof feedback);
}

void onUDPSocketData(void* buffer, int size) {
    if (sizeof(SpeedCommand) == size) {
        failSafeCountMotors = 0;
        returnSpeeds = true;

        SpeedCommand *command = static_cast<SpeedCommand *>(buffer);

        motors.setSpeeds(command->speed1, command->speed2, command->speed3, command->speed4, command->speed5);
    }
}

void handleSpeedsSent() {
    if (returnSpeeds) {
        isSpeedChanged = 1;
        sendFeedback();
    }
}

int main() {
    motors.baud(150000);
    motors.attach(&handleSpeedsSent);

    tfMini.baud(115200);
    led1 = 0;
    led2 = 0;
    led3 = 1;

    eth.set_network(MBED_IP_ADDRESS, "255.255.255.0", PC_IP_ADDRESS);
    eth.connect();

    socket.set_blocking(false);
    socket.open(&eth);
    socket.bind(PORT);

    SocketAddress address;

    heartbeatTicker.attach_us(&heartbeatTick, heartBeatPeriod_us);

    bool blinkState = false;

    led2 = 1;
    led3 = 0;

    runningTime.start();

    while (true) {
        led2 = 1;
        motors.update();
        led2 = 0;

        if (isHeartbeatUpdate) {
            failSafeCountMotors++;
            isHeartbeatUpdate = false;

            if (failSafeCountMotors == failSafeLimitMotors) {
                failSafeCountMotors = 0;

                if (failSafeEnabled) {
                    returnSpeeds = false;
                    motors.setSpeeds(0, 0, 0, 0, 0);
                }
            }

            if (updateLeds) {
                updateLeds = false;

                if (blinkState) {
                    led1 = 1;
                    sendFeedback();
                } else {
                    led1 = 0;
                }

                blinkState = !blinkState;
            }

        }

        nsapi_size_or_error_t size = socket.recvfrom(&address, recvBuffer, sizeof recvBuffer);

        if (size > 0) {
            recvBuffer[size] = '\0';
            //pc.printf("recv %d [%s] from %s:%d\n", size, recvBuffer, address.get_ip_address(), address.get_port());

            onUDPSocketData(recvBuffer, size);
        }

        int newBall1State = ball1;
        bool isBallStateChanged = false;

        if (ball1State != newBall1State) {
            ball1State = newBall1State;
            isBallStateChanged = true;
        }

        int newBall2State = ball2;

        if (ball2State != newBall2State) {
            ball2State = newBall2State;
            isBallStateChanged = true;
        }

        if (isBallStateChanged) {
            sendFeedback();
        }
    }
}