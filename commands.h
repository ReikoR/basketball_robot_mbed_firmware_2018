#ifndef BBR18_RK_MBED_FIRMWARE_COMMANDS_H
#define BBR18_RK_MBED_FIRMWARE_COMMANDS_H

struct SpeedCommand {
    int16_t speed1;
    int16_t speed2;
    int16_t speed3;
    int16_t speed4;
    int16_t speed5;
};

struct __attribute__((packed)) Feedback {
    int16_t speed1;
    int16_t speed2;
    int16_t speed3;
    int16_t speed4;
    int16_t speed5;
    uint8_t ball1;
    uint8_t ball2;
    uint16_t distance;
    uint8_t isSpeedChanged;
    int time;
};

#endif //BBR18_RK_MBED_FIRMWARE_COMMANDS_H
