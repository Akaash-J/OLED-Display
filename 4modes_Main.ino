#include "Config.h"
#include "Display.h"
#include "Button.h"
#include "Stopwatch.h"

// Modules
Display display;
Button modeButton(buttonPin);
Button stopwatchButton(stopwatchButtonPin);
Stopwatch stopwatch;

// Current mode tracking
int currentMode = 1;  // Start in stopwatch mode
const int numModes = 4;  // Total number of modes

void handleStopwatchState();

void setup() {
    SerialUSB.begin(115200);

    // Initialize modules
    display.init();
    modeButton.init();
    stopwatchButton.init();

    display.displayMode(currentMode);  // Display initial mode
}

void loop() {
    // Handle mode change button
    if (modeButton.isPressed()) {
        currentMode = (currentMode + 1) % numModes;  // Cycle through modes
        display.displayMode(currentMode);  // Update the display
    }

    // Handle mode-specific functions
    switch (currentMode) {
        case 0:
            // Speed/Distance Mode
            SerialUSB.println("In Speed/Distance Mode");
            break;
        case 1:
            handleStopwatchState();
            break;
        case 2:
            // Lap Timing Mode
            SerialUSB.println("In Lap Timing Mode");
            break;
        case 3:
            // Overall Mode
            SerialUSB.println("In Overall Mode");
            break;
    }

    delay(100);  // Small delay for smoother operation
}

void handleStopwatchState() {
    if (stopwatchButton.isPressed()) {
        // Handle stopwatch button presses
        if (stopwatch.isReset()) {
            stopwatch.start();
        } else if (stopwatch.isRunning()) {
            stopwatch.pause();
        } else {
            stopwatch.reset();
        }
    }

    // Display the stopwatch time on the screen
    unsigned long elapsedTime = stopwatch.getElapsedTime();
    unsigned long totalSeconds = elapsedTime / 1000;
    unsigned long hours = totalSeconds / 3600;
    unsigned long minutes = (totalSeconds % 3600) / 60;
    unsigned long seconds = totalSeconds % 60;
    unsigned long milliseconds = elapsedTime % 1000;

    display.displayStopwatchTime(hours, minutes, seconds, milliseconds);
}
