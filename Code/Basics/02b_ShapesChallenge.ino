/*
  GameCraft - Lesson 2b: Challenge - Draw Your Own Character!
  --------------------------------------------------------------
  Try to recreate this simple robot face using only shapes.
  Compare your result with the reference in comments below.
*/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define OLED_MOSI     10
#define OLED_CLK      8
#define OLED_DC       7
#define OLED_CS       5
#define OLED_RST      9

Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

void setup() {
  display.begin(0, true);
  display.clearDisplay();

  int x = 64, y = 32; // center of screen

  // Head (rounded rectangle)
  display.drawRoundRect(x - 20, y - 20, 40, 40, 6, SH110X_WHITE);

  // Eyes (filled circles)
  display.fillCircle(x - 8, y - 5, 3, SH110X_WHITE);
  display.fillCircle(x + 8, y - 5, 3, SH110X_WHITE);

  // Mouth (line)
  display.drawLine(x - 8, y + 10, x + 8, y + 10, SH110X_WHITE);

  // Antenna (line + circle)
  display.drawLine(x, y - 20, x, y - 28, SH110X_WHITE);
  display.fillCircle(x, y - 30, 2, SH110X_WHITE);

  display.display();
}

void loop() {
  // Static robot face
}

/*
  🎯 CHALLENGE: 
  Now try modifying this to create YOUR OWN character:
  - A cat face?
  - A spaceship?
  - Your own game's main character?
  
  This is exactly the skill you'll use to design characters 
  for the games in the next lessons!
*/
