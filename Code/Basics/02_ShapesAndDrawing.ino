/*
  GameCraft - Lesson 2: Shapes and Drawing
  ------------------------------------------
  Goal: Learn all the basic shape-drawing functions available in Adafruit_GFX.
  
  Concepts covered:
  - drawPixel() - single dot
  - drawLine() - straight lines
  - drawRect() / fillRect() - rectangles (outline vs filled)
  - drawCircle() / fillCircle() - circles
  - drawTriangle() / fillTriangle() - triangles
  - drawRoundRect() / fillRoundRect() - rounded rectangles
  
  Why this matters: Every game object (bird, pipes, obstacles, UI) 
  is just a combination of these basic shapes!
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

  // ---------------------------------------------------
  // 1. drawPixel(x, y, color) - the most basic building block
  // ---------------------------------------------------
  display.drawPixel(5, 5, SH110X_WHITE);
  display.drawPixel(7, 5, SH110X_WHITE);
  display.drawPixel(9, 5, SH110X_WHITE);

  // ---------------------------------------------------
  // 2. drawLine(x0, y0, x1, y1, color) - connects two points
  // ---------------------------------------------------
  display.drawLine(0, 15, 30, 15, SH110X_WHITE);   // horizontal line
  display.drawLine(35, 10, 35, 20, SH110X_WHITE);  // vertical line
  display.drawLine(40, 10, 60, 20, SH110X_WHITE);  // diagonal line

  // ---------------------------------------------------
  // 3. Rectangles - outline vs filled
  // Syntax: drawRect(x, y, width, height, color)
  // ---------------------------------------------------
  display.drawRect(65, 8, 20, 14, SH110X_WHITE);        // hollow rectangle
  display.fillRect(90, 8, 20, 14, SH110X_WHITE);        // filled rectangle

  // ---------------------------------------------------
  // 4. Circles
  // Syntax: drawCircle(centerX, centerY, radius, color)
  // ---------------------------------------------------
  display.drawCircle(15, 35, 8, SH110X_WHITE);          // hollow circle
  display.fillCircle(40, 35, 8, SH110X_WHITE);          // filled circle

  // ---------------------------------------------------
  // 5. Triangles
  // Syntax: drawTriangle(x0,y0, x1,y1, x2,y2, color)
  // ---------------------------------------------------
  display.drawTriangle(60, 42, 70, 28, 80, 42, SH110X_WHITE);   // hollow
  display.fillTriangle(90, 42, 100, 28, 110, 42, SH110X_WHITE); // filled

  // ---------------------------------------------------
  // 6. Rounded Rectangles - great for buttons/UI panels
  // Syntax: drawRoundRect(x, y, width, height, radius, color)
  // ---------------------------------------------------
  display.drawRoundRect(5, 50, 30, 12, 4, SH110X_WHITE);
  display.fillRoundRect(40, 50, 30, 12, 4, SH110X_WHITE);

  display.display();
}

void loop() {
  // Static for now - we'll animate shapes in Lesson 3
}

/*
  🧪 TRY THIS YOURSELF:
  1. Change fillCircle radius from 8 to 15 - what happens if it goes off screen?
  2. Try drawing your own simple character using 2-3 shapes combined 
     (e.g., a circle for head + rectangle for body).
  3. Change some SH110X_WHITE to SH110X_BLACK - notice how BLACK 
     can be used to "erase" or create cutout effects on white shapes.
  4. Try combining a fillCircle + smaller fillCircle in BLACK on top 
     to create a "donut" or "eye" effect (this is exactly how the 
     bird's eye was made in Flappy Bird!).

  📌 KEY INSIGHT:
  Complex game sprites (like the Flappy Bird or Dino character) 
  are NOT images - they're just multiple shapes layered together!
  
  Example - a simple "bird" made of shapes:
    fillCircle(x, y, 3, WHITE)              -> body
    fillTriangle(x+3,y, x+6,y-1, x+6,y+1, WHITE)  -> beak
    fillCircle(x+1, y-1, 1, BLACK)          -> eye (drawn in BLACK to "cut into" the white body)
*/
