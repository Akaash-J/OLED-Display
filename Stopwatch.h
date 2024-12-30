#ifndef STOPWATCH_H
#define STOPWATCH_H

enum StopwatchState { RESET, RUNNING, PAUSED };

class Stopwatch {
public:
    StopwatchState state;
    unsigned long startTime;
    unsigned long elapsedTime;

    Stopwatch() : state(RESET), startTime(0), elapsedTime(0) {}

    void start() {
        if (state == RESET || state == PAUSED) {
            state = RUNNING;
            startTime = millis() - elapsedTime;  // Start from where it was paused
        }
    }

    void pause() {
        if (state == RUNNING) {
            state = PAUSED;
            elapsedTime = millis() - startTime;  // Save the elapsed time
        }
    }

    void reset() {
        state = RESET;
        startTime = 0;
        elapsedTime = 0;
    }

    unsigned long getElapsedTime() {
        if (state == RUNNING) {
            return millis() - startTime;
        }
        return elapsedTime;
    }

    bool isRunning() {
        return state == RUNNING;
    }

    bool isReset() {
        return state == RESET;
    }
};

#endif
