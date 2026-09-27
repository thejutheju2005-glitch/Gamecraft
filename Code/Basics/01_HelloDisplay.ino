/*
  GameCraft - Lesson 1: Hello Display
  ------------------------------------
  Goal: Learn how to initialize the OLED display and print text.
  
  Concepts covered:
  - Display initialization
  - setCursor() - positioning text
  - setTextSize() - changing font size
  - setTextColor() - text color (WHITE/BLACK on monochrome OLED)
  - print() vs println()
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// OLED SPI Pins (same wiring for all GameCraft projects)
#define OLED_MOSI     10
#define OLED_CLK      8
#define OLED_DC       7
#define OLED_CS       5
#define OLED_RST      9

Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

void setup() {
  // Step 1: Always initialize the display first
  display.begin(0, true);

  // Step 2: Clear any leftover data on screen
  display.clearDisplay();

  // Step 3: Set text properties
  display.setTextSize(1);              // Try changing this to 2 or 3!
  display.setTextColor(SH110X_WHITE);  // Text color (white pixels on black background)

  // Step 4: Position your text using X, Y coordinates
  // (0,0) is the TOP-LEFT corner of the screen
  display.setCursor(0, 0);
  display.print("Hello, GameCraft!");

  // Step 5: Print more text on a new line
  display.setCursor(0, 10); // Move down 10 pixels for next line
  display.print("Line 2 of text");

  // Try larger text size
  display.setTextSize(2);
  display.setCursor(0, 30);
  display.print("BIG TEXT");

  // Step 6: IMPORTANT - display.display() actually pushes 
  // everything you drew onto the physical screen.
  // Without this line, nothing will show up!
  display.display();
}

void loop() {
  // Nothing here yet - the text stays on screen since we only draw once.
  // In later lessons, we'll update the display continuously inside loop().
}

/*
  🧪 TRY THIS YOURSELF:
  1. Change the text strings to your name.
  2. Change setTextSize(1) to setTextSize(3) - what happens to spacing?
  3. Try setCursor(64, 30) - what changes?
  4. Try changing SH110X_WHITE to SH110X_BLACK - what do you see? (Hint: it becomes invisible against black background)
  
  📌 SCREEN COORDINATE SYSTEM:
  (0,0) ---------------- (127,0)
    |                        |
    |      128 x 64          |
    |       screen           |
    |                        |
  (0,63) --------------- (127,63)
*/
