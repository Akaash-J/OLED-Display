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

// Stopwatch and switch pin setup
#define stopwatchButtonPin 2  // New button for stopwatch

volatile bool stopwatchPressed = false;  // Track stopwatch button press
enum StopwatchState { RESET, RUNNING, STOPPED, DISPLAY_ZERO };
StopwatchState stopwatchState = RESET;    // Track the current state of the stopwatch

unsigned long stopwatchStartTime = 0;
unsigned long stopwatchElapsedTime = 0;
unsigned long lastUpdate = 0;  // To limit the update rate of the OLED display

void handleStopwatchPress() {
    delay(50);
    if (digitalRead(stopwatchButtonPin) == LOW) {
        stopwatchPressed = true;
    }
}

void setup() {
    SerialUSB.begin(115200);
    while (!SerialUSB) { 
        ; // Wait for SerialUSB to initialize
    }
    SerialUSB.println("Serial USB initialized");  // Check if SerialUSB works

    // Initialize the OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC)) {
        SerialUSB.println(F("SSD1306 allocation failed"));
        for (;;);
    }
    display.clearDisplay();
    display.display();

    pinMode(stopwatchButtonPin, INPUT_PULLUP);  // New button for stopwatch
    attachInterrupt(digitalPinToInterrupt(stopwatchButtonPin), handleStopwatchPress, FALLING);  // Interrupt for stopwatch button

    displayStopwatchTime(0, 0, 0, 0);  // Initialize display with 0's
}

void loop() {
    if (stopwatchPressed) {
        stopwatchPressed = false;

        switch (stopwatchState) {
            case RESET:
                stopwatchState = DISPLAY_ZERO;
                break;
            case DISPLAY_ZERO:
                stopwatchState = RUNNING;
                stopwatchStartTime = millis();
                break;
            case RUNNING:
                stopwatchElapsedTime = millis() - stopwatchStartTime;
                stopwatchState = STOPPED;
                break;
            case STOPPED:
                stopwatchState = DISPLAY_ZERO;
                stopwatchStartTime = 0;
                stopwatchElapsedTime = 0;
                break;
        }
    }

    if (stopwatchState == RUNNING) {
        unsigned long currentMillis = millis();
        stopwatchElapsedTime = currentMillis - stopwatchStartTime;

        if (currentMillis - lastUpdate >= 100) {
            lastUpdate = currentMillis;

            unsigned long totalSeconds = stopwatchElapsedTime / 1000;
            unsigned long hours = totalSeconds / 3600;
            unsigned long minutes = (totalSeconds % 3600) / 60;
            unsigned long seconds = totalSeconds % 60;
            unsigned long milliseconds = stopwatchElapsedTime % 1000;

            // Update the stopwatch time on the OLED
            if (hours > 0) {
                displayStopwatchTime(hours, minutes, seconds, milliseconds);
            } else {
                displayStopwatchTime(0, minutes, seconds, milliseconds);
            }
        }
    } else if (stopwatchState == DISPLAY_ZERO) {
        // Display zero time on OLED
        displayStopwatchTime(0, 0, 0, 0);
    }

    delay(10);  // Small delay for debounce and smoother updates
}

void displayStopwatchTime(unsigned long hours, unsigned long minutes, unsigned long seconds, unsigned long milliseconds) {
    display.clearDisplay();
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
    display.setCursor(10, 20);
    display.print(timeString);

    display.display();
}
