#ifndef BATTERY_H
#define BATTERY_H

#include <Arduino.h>

#define BATTERY_PIN A0  // Define the pin for battery reading
#define NUM_READINGS 10
#define MIN_VOLTAGE 3.0
#define MAX_VOLTAGE 4.2

class Battery {
public:
    Battery();
    void begin();
    int getPercentage();

private:
    int readings[NUM_READINGS];
    int readIndex;
    int total;
    int average;
};

#endif
