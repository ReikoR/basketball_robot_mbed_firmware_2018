#ifndef BBR18_RK_MBED_FIRMWARE_MOTOR_H
#define BBR18_RK_MBED_FIRMWARE_MOTOR_H

#include "mbed.h"
#include "qed.h"

class Motor {
protected:
    FunctionPointer stallChangeCallback;
    FunctionPointer stallEndCallback;
    FunctionPointer stallWarningCallback;
    FunctionPointer stallErrorCallback;
public:
    /** Create an instance of the motor connected to specfied pins, and IO-expander.
     *
     * @param PWMpin Pin for PWM output
     * @param dir1 Direction 1 pin
     * @param dir2 Direction 2 pin
     * @param encA Encoder pin
     * @param encB Encoder pin
     */
    Motor(PinName PWMpin, PinName dir1Pin, PinName dir2Pin, PinName encA, PinName encB);

    /** Set speed setpoint
     *
     * @param newSpeed New setpoint
     */
    void setSpeed(int newSpeed);

    /** Get current speed setpoint value */
    int getSpeed();

    /**Method that calculates appropriate PWM values for keeping motor speed close to setpoint
    *     This method shoud be called periodically (60Hz)
    */
    void pid();

    int getStallLevel();

    void stallChange(void (*function)(void));

    template<typename T>
    void stallChange(T *object, void (T::*member)(void)) {
        stallChangeCallback.attach(object, member);
    }

    void stallEnd(void (*function)(void));

    template<typename T>
    void stallEnd(T *object, void (T::*member)(void)) {
        stallEndCallback.attach(object, member);
    }

    void stallWarning(void (*function)(void));

    template<typename T>
    void stallWarning(T *object, void (T::*member)(void)) {
        stallWarningCallback.attach(object, member);
    }

    void stallError(void (*function)(void));

    template<typename T>
    void stallError(T *object, void (T::*member)(void)) {
        stallErrorCallback.attach(object, member);
    }

private:
    PwmOut pwm;
    DigitalOut dir1;
    DigitalOut dir2;
    QED qed;

    int currentSpeed;
    int getDecoderCount();

    void resetPID();

    /** Set pwm duty cycle
     *
     * @param newPWM Duty cycle
     */
    void setPWM(int newPWM);

    //void pid();

    int setPoint;
    int pMulti;
    int iMulti;
    int dMulti;
    int error;
    int prevError;
    int P;
    int I;
    int D;
    int minPwm;
    int pidMulti;
    int iMax;

    int pwmPeriod;

    int currentPWM;
    int stallCount;
    int prevStallCount;
    int stallWarningLimit;
    int stallErrorLimit;
    int stallLevel;
};

#endif //BBR18_RK_MBED_FIRMWARE_MOTOR_H
