#include "LedManager.h"

LedManager::LedManager(PinName dataPin): inputForPullDown(dataPin, PullDown), pixelArray(dataPin) {
    // Create a temporary DigitalIn so we can configure the pull-down resistor.
    // (The mbed API doesn't provide any other way to do this.)
    // An alternative is to connect an external pull-down resistor.
    //DigitalIn(dataPin, PullDown);

    // The pixel array control class.
    //pixelArray(dataPin);

    needsUpdate = true;
}

void LedManager::setLedColor(unsigned int ledIndex, Color color) {
    if (sizeof(pixels) > ledIndex) {
        switch (color) {
            case RED:
                pixels[ledIndex] = (struct neopixel::Pixel){40, 0, 0};
                break;
            case GREEN:
                pixels[ledIndex] = (struct neopixel::Pixel){0, 40, 0};
                break;
            case BLUE:
                pixels[ledIndex] = (struct neopixel::Pixel){0, 0, 40};
                break;
            case CYAN:
                pixels[ledIndex] = (struct neopixel::Pixel){0, 40, 40};
                break;
            case MAGENTA:
                pixels[ledIndex] = (struct neopixel::Pixel){40, 0, 40};
                break;
            case YELLOW:
                pixels[ledIndex] = (struct neopixel::Pixel){40, 40, 0};
                break;
            case WHITE:
                pixels[ledIndex] = (struct neopixel::Pixel){40, 40, 40};
                break;
            case OFF:
                pixels[ledIndex] = (struct neopixel::Pixel){0, 0, 0};
                break;
        }

        needsUpdate = true;
    }
}

void LedManager::setLedCustomColor(unsigned int ledIndex, neopixel::Pixel pixel) {
    if (sizeof(pixels) > ledIndex) {
        pixels[ledIndex] = pixel;

        needsUpdate = true;
    }
}

void LedManager::update() {
    if (needsUpdate) {
        pixelArray.update(pixels, 2);

        needsUpdate = false;
    }
}