#include "motor.h"

Motor::Motor(PinName PWMpin, PinName dir1Pin, PinName dir2Pin, PinName encA, PinName encB)
        : pwm(PWMpin), dir1(dir1Pin), dir2(dir2Pin), qed(encA, encB) {

    pwmPeriod = 4000;
    PwmOut pwm(PWMpin);
    pwm.period_us(pwmPeriod);
    setPoint = 0;
    pMulti = 64;
    iMulti = 2;
    dMulti = 1;
    error = 0;
    prevError = 0;
    P = 0;
    I = 0;
    minPwm = 100;
    pidMulti = 256;
    iMax = 4096 * pidMulti;

    currentSpeed = 0;

    currentPWM = 0;
    stallCount = 0;
    prevStallCount = 0;
    stallWarningLimit = 60;
    stallErrorLimit = 300;
    stallLevel = 0;
}

void Motor::setPWM(int newPWM) {
    currentPWM = newPWM;
    if (newPWM < 0) {
        pwm.pulsewidth_us(-1 * newPWM);
        dir1 = 0;
        dir2 = 1;
    } else {
        pwm.pulsewidth_us(newPWM);
        dir1 = 1;
        dir2 = 0;
    }
}

int Motor::getSpeed() {
    return currentSpeed;
}

int Motor::getDecoderCount() {
    currentSpeed = qed.read();
    return currentSpeed;
}

void Motor::setSpeed(int newSpeed) {
    setPoint = newSpeed;

    if (newSpeed == 0) {
        resetPID();
    }
}

void Motor::pid() {
    prevError = error;
    error = setPoint - getDecoderCount();

    if (stallLevel != 2) {
        float minPwmValue = minPwm;

        if (setPoint == 0) {
            minPwmValue = 0;
        } else if (setPoint < 0) {
            minPwmValue = -minPwm;
        }

        I += error * pidMulti * iMulti;
        //constrain integral
        if (I < -iMax) I = -iMax;
        if (I > iMax) I = iMax;

        //D = error - prevError;

        int newPWMvalue = minPwmValue + error * pMulti + I / pidMulti;

        //constrain pwm
        if (newPWMvalue < -pwmPeriod) newPWMvalue = -pwmPeriod;
        if (newPWMvalue > pwmPeriod) newPWMvalue = pwmPeriod;

        prevStallCount = stallCount;
        if ((currentSpeed < 5 && currentPWM == pwmPeriod || currentSpeed > -5 && currentPWM == -pwmPeriod) && stallCount < stallErrorLimit) {
            stallCount++;
        } else if (stallCount > 0) {
            stallCount--;
        }

        setPWM(newPWMvalue);

        if ((stallCount == stallWarningLimit - 1) && (prevStallCount == stallWarningLimit)) {
            stallLevel = 0;
            stallEndCallback.call();
            stallChangeCallback.call();
        } else if ((stallCount == stallWarningLimit) && (prevStallCount == stallWarningLimit - 1)) {
            stallLevel = 1;
            stallWarningCallback.call();
            stallChangeCallback.call();
        } else if (stallCount == stallErrorLimit) {
            stallLevel = 2;
            stallErrorCallback.call();
            stallChangeCallback.call();
            resetPID();
        }
    } else {
        stallCount--;
        if (stallCount == 0) {
            stallLevel = 0;
            stallEndCallback.call();
            stallChangeCallback.call();
        }
    }
}

void Motor::resetPID() {
    error = 0;
    prevError = 0;
    P = 0;
    I = 0;
    setPoint = 0;
    setPWM(0);
}

int Motor::getStallLevel() {
    return stallLevel;
}

void Motor::stallChange(void (*function)(void)) {
    stallChangeCallback.attach(function);
}

void Motor::stallEnd(void (*function)(void)) {
    stallEndCallback.attach(function);
}

void Motor::stallWarning(void (*function)(void)) {
    stallWarningCallback.attach(function);
}

void Motor::stallError(void (*function)(void)) {
    stallErrorCallback.attach(function);
}