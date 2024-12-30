#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
public:
    Button(int pin) : pin(pin), lastButtonState(HIGH), lastDebounceTime(0), debounceDelay(50) {}

    void init() {
        pinMode(pin, INPUT_PULLUP);
    }

    bool isPressed() {
        int reading = digitalRead(pin);
        if (reading != lastButtonState) {
            lastDebounceTime = millis();
        }

        if ((millis() - lastDebounceTime) > debounceDelay) {
            if (reading != buttonState) {
                buttonState = reading;
                if (buttonState == LOW) {
                    return true;
                }
            }
        }

        lastButtonState = reading;
        return false;
    }

private:
    int pin;
    int buttonState;
    int lastButtonState;
    unsigned long lastDebounceTime;
    unsigned long debounceDelay;
};

#endif
