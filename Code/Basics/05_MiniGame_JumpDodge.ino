/*
  GameCraft - Lesson 5: CAPSTONE - Build Your First Complete Mini-Game!
  ------------------------------------------------------------------------
  Goal: Combine EVERYTHING from Lessons 1-4 into one real, playable game.
  
  This game uses:
  ✅ Lesson 1: Text (score display, game over screen)
  ✅ Lesson 2: Shapes (player character, obstacles made of rectangles/circles)
  ✅ Lesson 3: Animation (game loop: clear -> update -> draw -> display -> delay)
  ✅ Lesson 4: Button Input (simple read for jumping)
  
  🎯 After this lesson, you'll understand 100% of how every GameCraft 
  game works - because they're all built the EXACT same way, just with 
  more features added on top of this same foundation!
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

const int buttonPin = 2;

// ---------------------------------------------------
// GAME VARIABLES
// (Declared globally so they persist across loop() calls
//  - remember this from Lesson 3!)
// ---------------------------------------------------

// Player (simple stick figure using shapes from Lesson 2)
int playerX = 20;
int groundY = 50;
int playerY = groundY;
int playerVelocity = 0;
int gravity = 1;
int jumpForce = -8;
bool isJumping = false;

// Obstacle
int obstacleX = 128;
int obstacleWidth = 8;
int obstacleHeight = 12;
int gameSpeed = 3;

// Score & game state
int score = 0;
bool isGameOver = false;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  display.begin(0, true);
  display.clearDisplay();
  display.display();

  randomSeed(analogRead(0)); // for randomizing obstacle timing later
}

void loop() {
  if (!isGameOver) {
    updateGame();   // Lesson 3 & 4 concepts: move things, read input
    drawGame();     // Lesson 1 & 2 concepts: text + shapes
    delay(30);      // Lesson 3 concept: controls game speed
  } else {
    drawGameOverScreen();
    // Press button to restart
    if (digitalRead(buttonPin) == HIGH) {
      resetGame();
    }
  }
}

// =====================================================
// UPDATE: Handle input, physics, and game logic
// =====================================================
void updateGame() {
  // ---- BUTTON INPUT (Lesson 4 - simple read) ----
  // We use simple continuous read here since jump-when-not-already-jumping
  // naturally prevents double-jumping without needing edge detection
  if (digitalRead(buttonPin) == HIGH && !isJumping) {
    playerVelocity = jumpForce;
    isJumping = true;
  }

  // ---- PHYSICS (simple gravity simulation) ----
  playerY += playerVelocity;
  playerVelocity += gravity;

  // Don't let player fall through the ground
  if (playerY >= groundY) {
    playerY = groundY;
    playerVelocity = 0;
    isJumping = false;
  }

  // ---- MOVE OBSTACLE (Lesson 3 - animation) ----
  obstacleX -= gameSpeed;

  // If obstacle goes off-screen, reset it and increase score
  if (obstacleX < -obstacleWidth) {
    obstacleX = 128 + random(0, 40); // slight randomness for variety
    score++;

    // Every 5 points, speed up slightly (progressive difficulty)
    if (score % 5 == 0) {
      gameSpeed++;
    }
  }

  // ---- COLLISION DETECTION ----
  // Check if player's bounding box overlaps obstacle's bounding box
  int playerLeft = playerX;
  int playerRight = playerX + 8;
  int playerTop = playerY - 12;
  int playerBottom = playerY;

  int obstacleLeft = obstacleX;
  int obstacleRight = obstacleX + obstacleWidth;
  int obstacleTop = groundY - obstacleHeight;
  int obstacleBottom = groundY;

  bool overlapX = playerRight > obstacleLeft && playerLeft < obstacleRight;
  bool overlapY = playerBottom > obstacleTop && playerTop < obstacleBottom;

  if (overlapX && overlapY) {
    isGameOver = true;
  }
}

// =====================================================
// DRAW: Everything visual (Lesson 1 text + Lesson 2 shapes)
// =====================================================
void drawGame() {
  display.clearDisplay();

  // ---- Ground line ----
  display.drawLine(0, groundY, 128, groundY, SH110X_WHITE);

  // ---- Player (simple stick figure using basic shapes) ----
  // Head
  display.fillCircle(playerX + 4, playerY - 10, 3, SH110X_WHITE);
  // Body
  display.drawLine(playerX + 4, playerY - 7, playerX + 4, playerY - 2, SH110X_WHITE);
  // Legs
  display.drawLine(playerX + 4, playerY - 2, playerX + 1, playerY, SH110X_WHITE);
  display.drawLine(playerX + 4, playerY - 2, playerX + 7, playerY, SH110X_WHITE);

  // ---- Obstacle (simple rectangle) ----
  display.fillRect(obstacleX, groundY - obstacleHeight, obstacleWidth, obstacleHeight, SH110X_WHITE);

  // ---- Score text ----
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(90, 0);
  display.print("Score:");
  display.setCursor(95, 10);
  display.print(score);

  display.display();
}

// =====================================================
// GAME OVER SCREEN
// =====================================================
void drawGameOverScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  display.setCursor(30, 15);
  display.print("GAME OVER!");

  display.setCursor(25, 30);
  display.print("Score: ");
  display.print(score);

  display.setCursor(10, 45);
  display.print("Press button to retry");

  display.display();
}

// =====================================================
// RESET: Put all variables back to starting values
// =====================================================
void resetGame() {
  playerY = groundY;
  playerVelocity = 0;
  isJumping = false;
  obstacleX = 128;
  gameSpeed = 3;
  score = 0;
  isGameOver = false;
  delay(200); // small debounce so button press doesn't immediately re-trigger
}

/*
  🎉 CONGRATULATIONS! You just built a complete game from scratch!
  
  🧪 NOW TRY THESE MODIFICATIONS TO MAKE IT YOUR OWN:
  1. Change jumpForce from -8 to -12 - jump higher!
  2. Change obstacleHeight to random(8, 20) for varying obstacle sizes
  3. Add a second obstacle (hint: look at how Dino Runner in the main 
     game library handles TWO obstacles using an array)
  4. Change the stick figure to your own character using shapes 
     from Lesson 2
  5. Add a "high score" variable that persists across resetGame() calls
     (hint: don't reset the high score variable itself!)

  📌 YOU'RE READY FOR THE FULL GAME LIBRARY!
  Every game in GameCraft (Flappy Bird, Dino Runner, Heli Cave, 
  Stop The Bar, Memory Beep, Rhythm Tap) uses this EXACT same 
  structure you just built:
  
    setup() -> initialize everything once
    loop()  -> check game state, call updateGame() + drawGame()
    updateGame() -> physics, input, collision, scoring
    drawGame()   -> clear, draw shapes/text, display()
    resetGame()  -> reset variables for a new attempt
  
  They just have MORE features (animations, multiple obstacles, 
  sound-like feedback, levels) built on top of this same foundation.
  
  Go explore the /02_Games folder now - you'll recognize this 
  pattern everywhere!
*/
