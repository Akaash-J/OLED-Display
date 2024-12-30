#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// Button and Mode Setup
#define buttonPin 8  // Pin for the push button
int currentMode = 0;  // 0: spd/dis, 1: stopwatch, 2: lap timing, 3: overall
const int numModes = 4;  // Total number of modes
#define stopwatchButtonPin 2  // New button for stopwatch

volatile bool stopwatchPressed = false;  // Track stopwatch button press
enum StopwatchState { RESET, RUNNING, PAUSED };
StopwatchState stopwatchState = RESET;    // Track the current state of the stopwatch

unsigned long stopwatchStartTime = 0;     // Holds the time when the stopwatch started
unsigned long stopwatchElapsedTime = 0;   // Holds the total elapsed time
unsigned long lastUpdate = 0;  // To limit the update rate of the OLED display

unsigned long lastButtonPress = 0;        // For debouncing the button
bool buttonState = HIGH;
bool lastButtonState = HIGH;

void handleStopwatchPress() {
    delay(50);  // Debounce
    if (digitalRead(stopwatchButtonPin) == LOW) {
        stopwatchPressed = true;
    }
}

void setup() {
    SerialUSB.begin(115200);
    pinMode(buttonPin, INPUT_PULLUP);  // Set the button pin as input with internal pull-up resistor

    // Initialize the OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC)) {
        SerialUSB.println(F("SSD1306 allocation failed"));
        for (;;);
    }
    display.clearDisplay();
    display.display();
    
    pinMode(stopwatchButtonPin, INPUT_PULLUP);  // New button for stopwatch
    attachInterrupt(digitalPinToInterrupt(stopwatchButtonPin), handleStopwatchPress, FALLING);  // Interrupt for stopwatch button

    displayMode();  // Show initial mode
}

void loop() {
    handleButtonPress();  // Handle mode change by button press

    // Call functions based on the current mode
    switch (currentMode) {
        case 0:
            modeSpdDis();
            break;
        case 1:
            modeStopwatch();
            break;
        case 2:
            modeLapTiming();
            break;
        case 3:
            modeOverall();
            break;
    }

    delay(100);  // Small delay for smoother operation
}

void handleButtonPress() {
    buttonState = digitalRead(buttonPin);

    if (buttonState == LOW && lastButtonState == HIGH && (millis() - lastButtonPress) > 200) {  // Debounce
        lastButtonPress = millis();
        currentMode = (currentMode + 1) % numModes;  // Change to the next mode
        displayMode();  // Update the display to show the new mode
    }

    lastButtonState = buttonState;
}

void displayMode() {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);

    // Display current mode at the top of the screen
    switch (currentMode) {
        case 0:
            display.setCursor(10, 0);
            display.print("Mode: Spd/Dis");
            break;
        case 1:
            display.setCursor(10, 0);
            display.print("Mode: Stopwatch");
            break;
        case 2:
            display.setCursor(10, 0);
            display.print("Mode: Lap Timing");
            break;
        case 3:
            display.setCursor(10, 0);
            display.print("Mode: Overall");
            break;
    }
    display.display();  // Only update the top portion of the display
}

void modeSpdDis() {
    // Add functionality for Speed/Distance mode
    SerialUSB.println("In Speed/Distance Mode");
}

void modeStopwatch() {
    // Handle the stopwatch logic
    if (stopwatchPressed) {
        stopwatchPressed = false;

        // Handle the stopwatch state transitions
        switch (stopwatchState) {
            case RESET:
                stopwatchState = RUNNING;
                stopwatchStartTime = millis() - stopwatchElapsedTime;  // Start from the elapsed time
                break;

            case RUNNING:
                stopwatchState = PAUSED;
                stopwatchElapsedTime = millis() - stopwatchStartTime;  // Save the current elapsed time
                break;

            case PAUSED:
                stopwatchState = RESET;
                stopwatchElapsedTime = 0;
                break;
        }
    }

    if (stopwatchState == RUNNING) {
        unsigned long currentMillis = millis();
        stopwatchElapsedTime = currentMillis - stopwatchStartTime;

        if (currentMillis - lastUpdate >= 100) {  // Update every 100ms
            lastUpdate = currentMillis;

            unsigned long totalSeconds = stopwatchElapsedTime / 1000;
            unsigned long hours = totalSeconds / 3600;
            unsigned long minutes = (totalSeconds % 3600) / 60;
            unsigned long seconds = totalSeconds % 60;
            unsigned long milliseconds = stopwatchElapsedTime % 1000;

            // Display the stopwatch time
            displayStopwatchTime(hours, minutes, seconds, milliseconds);
        }
    } else if (stopwatchState == PAUSED || stopwatchState == RESET) {
        // Display the last elapsed time or reset time
        unsigned long totalSeconds = stopwatchElapsedTime / 1000;
        unsigned long hours = totalSeconds / 3600;
        unsigned long minutes = (totalSeconds % 3600) / 60;
        unsigned long seconds = totalSeconds % 60;
        unsigned long milliseconds = stopwatchElapsedTime % 1000;

        displayStopwatchTime(hours, minutes, seconds, milliseconds);
    }
}

void modeLapTiming() {
    SerialUSB.println("In Lap Timing Mode");
}

void modeOverall() {
    SerialUSB.println("In Overall Mode");
}

void displayStopwatchTime(unsigned long hours, unsigned long minutes, unsigned long seconds, unsigned long milliseconds) {
    // Make sure we are not clearing the whole display
    display.fillRect(0, 10, SCREEN_WIDTH, SCREEN_HEIGHT - 10, SSD1306_BLACK);  // Clear only the timer area
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    
    // Create a buffer to hold the formatted time string
    char timeString[20];
    if (hours > 0) {
        snprintf(timeString, sizeof(timeString), "%02lu:%02lu:%02lu.%03lu", hours, minutes, seconds, milliseconds);
    } else {
        snprintf(timeString, sizeof(timeString), "   %02lu:%02lu.%03lu", minutes, seconds, milliseconds);
    }

    // Display the formatted time at a fixed location on the OLED
    display.setCursor(10, 30);  // Adjust y position to the bottom of the display
    display.print(timeString);
    display.display();
}
