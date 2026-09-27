/*
  GameCraft - Lesson 3: Animation - Moving a Shape
  ---------------------------------------------------
  Goal: Learn the core "Game Loop" pattern used in EVERY game.
  
  Concepts covered:
  - The Clear -> Update -> Draw -> Display cycle
  - Using variables to track position
  - Using delay() to control animation speed
  - Basic screen boundary detection (bouncing off edges)
  
  🎯 THIS IS THE MOST IMPORTANT LESSON!
  Every single game (Flappy Bird, Dino Runner, etc.) is built using
  this exact same repeating pattern you're about to learn.
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

// ---------------------------------------------------
// STEP 1: Variables to track our shape's position
// These live OUTSIDE setup()/loop() so they persist 
// between loop() calls (this is critical!)
// ---------------------------------------------------
int ballX = 10;
int ballY = 32;
int ballRadius = 5;

// Direction/speed - how much to move each frame
int speedX = 2;
int speedY = 1;

void setup() {
  display.begin(0, true);
  display.clearDisplay();
  display.display();
}

void loop() {
  // ==================================================
  // THE GAME LOOP PATTERN - memorize this structure!
  // 1. CLEAR  - erase the previous frame
  // 2. UPDATE - change variables (move things, check logic)
  // 3. DRAW   - draw everything at its NEW position
  // 4. DISPLAY- push the frame to the physical screen
  // 5. DELAY  - control how fast the game runs
  // ==================================================

  // ---- 1. CLEAR ----
  display.clearDisplay();

  // ---- 2. UPDATE ----
  ballX += speedX;  // move ball horizontally
  ballY += speedY;  // move ball vertically

  // Boundary detection - bounce off screen edges
  if (ballX - ballRadius <= 0 || ballX + ballRadius >= 128) {
    speedX = -speedX; // reverse horizontal direction
  }
  if (ballY - ballRadius <= 0 || ballY + ballRadius >= 64) {
    speedY = -speedY; // reverse vertical direction
  }

  // ---- 3. DRAW ----
  display.fillCircle(ballX, ballY, ballRadius, SH110X_WHITE);

  // Optional: show position values on screen for learning purposes
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.print("X:");
  display.print(ballX);
  display.setCursor(0, 8);
  display.print("Y:");
  display.print(ballY);

  // ---- 4. DISPLAY ----
  display.display();

  // ---- 5. DELAY ----
  delay(30); // smaller = faster animation, larger = slower
}

/*
  🧪 TRY THIS YOURSELF:
  1. Change delay(30) to delay(100) - notice how much slower it moves?
     This is EXACTLY how "speed" is controlled in all our games.
  
  2. Change speedX = 2 to speedX = 5 - the ball moves faster per frame.
  
  3. Change fillCircle to fillRect - now a square bounces instead!
     (Hint: you'll need to adjust the boundary math slightly since 
     rectangles use x,y as top-left corner, not center)
  
  4. Try removing the "CLEAR" step (comment out display.clearDisplay()) 
     - what happens? You'll see why clearing every frame is essential!
  
  5. Add a second ball with its own X/Y/speed variables - can you get 
     two balls bouncing independently?

  📌 KEY INSIGHT - CONNECT TO REAL GAMES:
  Look back at Flappy Bird's updateGame() function:
    - "birdY += velocity" is EXACTLY like "ballY += speedY" here
    - "if (birdY < 0 || birdY > 64) gameOver = true" is the same 
      boundary check you just wrote, just used for game-over instead 
      of bouncing!
  
  You now understand the CORE mechanic behind every game we've built.
*/
