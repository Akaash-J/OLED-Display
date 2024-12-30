#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "Config.h"  // Include the configuration file

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

class Display {
public:
    Adafruit_SSD1306 display;

    Display() : display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS) {}

    void init() {
        if (!display.begin(SSD1306_SWITCHCAPVCC)) {
            SerialUSB.println(F("SSD1306 allocation failed"));
            for (;;);  // Don't proceed, loop forever
        }
        display.clearDisplay();
        display.display();
    }

    void displayMode(int currentMode) {
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);

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
        display.display();
    }

    void displayStopwatchTime(unsigned long hours, unsigned long minutes, unsigned long seconds, unsigned long milliseconds) {
        display.fillRect(0, 10, SCREEN_WIDTH, SCREEN_HEIGHT - 10, SSD1306_BLACK);  // Clear only the timer area
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);

        char timeString[20];
        if (hours > 0) {
            snprintf(timeString, sizeof(timeString), "%02lu:%02lu:%02lu.%03lu", hours, minutes, seconds, milliseconds);
        } else {
            snprintf(timeString, sizeof(timeString), "   %02lu:%02lu.%03lu", minutes, seconds, milliseconds);
        }

        display.setCursor(10, 30);  // Adjust y position to the bottom of the display
        display.print(timeString);
        display.display();
    }
};

#endif
