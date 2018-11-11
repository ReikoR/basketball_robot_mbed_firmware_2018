#ifndef BBR18_RK_MBED_FIRMWARE_COMMANDS_H
#define BBR18_RK_MBED_FIRMWARE_COMMANDS_H

struct __attribute__((packed)) RobotCommand {
    int16_t speed1;
    int16_t speed2;
    int16_t speed3;
    int16_t speed4;
    int16_t speed5;
    int16_t speed6;
    uint16_t servo;
    char fieldID;
    char robotID;
    uint8_t shouldSendAck;
};

struct __attribute__((packed)) Feedback {
    int16_t speed1;
    int16_t speed2;
    int16_t speed3;
    int16_t speed4;
    int16_t speed5;
    int16_t speed6;
    uint8_t ball1;
    uint8_t ball2;
    uint8_t isSpeedChanged;
    char refereeCommand;
    int time;
};

#endif //BBR18_RK_MBED_FIRMWARE_COMMANDS_H
