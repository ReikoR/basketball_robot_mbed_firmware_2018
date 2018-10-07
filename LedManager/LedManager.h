#ifndef MBED_TEST_LEDMANAGER_H
#define MBED_TEST_LEDMANAGER_H

#include "mbed.h"
#include "neopixel.h"

class LedManager {
public:
    LedManager(PinName dataPin);

    enum Color {RED, GREEN, BLUE, CYAN, MAGENTA, YELLOW, WHITE, OFF};

    void setLedColor(unsigned int ledIndex, Color color);
    void setLedCustomColor(unsigned int ledIndex, neopixel::Pixel pixel);

    void update();
private:
    DigitalIn inputForPullDown;

    neopixel::PixelArray pixelArray;

    neopixel::Pixel pixels[2];

    bool needsUpdate;
};

#endif //MBED_TEST_LEDMANAGER_H
