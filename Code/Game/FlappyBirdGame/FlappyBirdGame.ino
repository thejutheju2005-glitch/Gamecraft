#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// OLED SPI Pins
#define OLED_MOSI     10
#define OLED_CLK      8
#define OLED_DC       7
#define OLED_CS       5
#define OLED_RST      9

Adafruit_SH1106G display = Adafruit_SH1106G(128, 64, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RST, OLED_CS);

// Game variables
int birdY = 32;
int velocity = 0;
int gravity = 1;
int jump = -4;
int pipeX[3];
int gapYArr[3];
int pipeDir[3]; // for vertical movement
int coinX = -20, coinY = 0;
bool coinActive = false;
int gapHeight = 20;
int score = 0;
int level = 1;
int speed = 2;
bool gameOver = false;
bool gameStarted = false;

const int buttonPin = 2;
unsigned long lastFlapTime = 0;
bool flapFrame = false;  // for bird animation

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  display.begin(0, true);
  display.clearDisplay();
  display.display();

  randomSeed(analogRead(0));
  for (int i = 0; i < 3; i++) {
    pipeX[i] = 128 + i * 50;
    gapYArr[i] = random(10, 44);
    pipeDir[i] = random(0, 2) ? 1 : -1;
  }
}

void loop() {
  if (!gameStarted) {
    drawStartScreen();
    if (digitalRead(buttonPin) == HIGH) { // If the button used is Normally Closed, then compare with HIGH. Else, compare with LOW
      gameStarted = true;
      delay(200);
    }
  } else if (!gameOver) {
    updateGame();
    drawGame();
    delay(30);
  } else {
    drawGameOver();
    if (digitalRead(buttonPin) == HIGH) { // If the button used is Normally Closed, then compare with HIGH. Else, compare with LOW
      resetGame();
    }
  }
}

// --- REALISTIC BIRD DRAWING ---
void drawBird(int x, int y) {
  // Body
  display.fillCircle(x, y, 3, SH110X_WHITE);
  // Beak
  display.fillTriangle(x+3, y, x+6, y-1, x+6, y+1, SH110X_WHITE);
  // Eye
  display.fillCircle(x+1, y-1, 1, SH110X_BLACK);
  // Wing (animated)
  if (flapFrame)
    display.fillTriangle(x-2, y, x-4, y-2, x-4, y+2, SH110X_WHITE);
  else
    display.fillTriangle(x-2, y+2, x-4, y+4, x-1, y+4, SH110X_WHITE);
}

void updateGame() {
  // Bird movement
  if (digitalRead(buttonPin) == HIGH) velocity = jump; // If the button used is Normally Closed, then compare with HIGH. Else, compare with LOW
  velocity += gravity;
  birdY += velocity;

  // --- MULTIPLE PIPES & MOVING GAPS ---
  for (int i = 0; i < 3; i++) {
    pipeX[i] -= speed;
    // Move gap up/down for twist
    gapYArr[i] += pipeDir[i];
    if (gapYArr[i] < 10 || gapYArr[i] > 44) pipeDir[i] *= -1;

    if (pipeX[i] < -10) {
      pipeX[i] = 128;
      gapYArr[i] = random(10, 44);
      pipeDir[i] = random(0, 2) ? 1 : -1;
      score++;
      if (score % 5 == 0) { // Increase difficulty every 5 points
        level++;
        speed++;
        if (gapHeight > 10) gapHeight--; // reduce gap size for challenge
      }
    }
  }

  // --- BONUS COIN LOGIC ---
  if (!coinActive && random(0, 100) < 2) { // 2% chance per frame
    coinX = 128;
    coinY = random(10, 54);
    coinActive = true;
  }
  if (coinActive) {
    coinX -= speed;
    if (coinX < -5) coinActive = false;
    // Collision with bird
    if (abs(coinX-10) < 5 && abs(coinY-birdY) < 5) {
      score += 3; // bonus
      coinActive = false;
    }
  }

  // Bird animation toggle every 100ms
  if (millis() - lastFlapTime > 100) {
    flapFrame = !flapFrame;
    lastFlapTime = millis();
  }

  // --- COLLISION DETECTION FOR ALL PIPES ---
  for (int i = 0; i < 3; i++) {
    if (pipeX[i] < 14 && pipeX[i] > 0 &&
        (birdY < gapYArr[i] || birdY > gapYArr[i] + gapHeight)) {
      gameOver = true;
    }
  }
  if (birdY < 0 || birdY > 64) gameOver = true;
}

void drawGame() {
  display.clearDisplay();

  // Draw bird
  drawBird(10, birdY);

  // Draw all pipes
  for (int i = 0; i < 3; i++) {
    display.fillRect(pipeX[i], 0, 10, gapYArr[i], SH110X_WHITE);
    display.fillRect(pipeX[i], gapYArr[i] + gapHeight, 10, 64 - gapYArr[i] - gapHeight, SH110X_WHITE);
  }

  // Draw coin
  if (coinActive)
    display.drawCircle(coinX, coinY, 3, SH110X_WHITE);

  // Score & Level
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(90, 0);
  display.print("S:");
  display.print(score);
  display.setCursor(90, 10);
  display.print("L:");
  display.print(level);

  display.display();
}

void drawStartScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(30, 20);
  display.print("FLAPPY BIRD");
  display.setCursor(20, 40);
  display.print("Press Button to");
  display.setCursor(45, 50);
  display.print("Start");
  display.display();
}

void drawGameOver() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(30, 20);
  display.print("GAME OVER");
  display.setCursor(30, 40);
  display.print("Score: ");
  display.print(score);
  display.display();
}

void resetGame() {
  birdY = 32;
  velocity = 0;
  score = 0;
  level = 1;
  speed = 2;
  gapHeight = 20;
  gameOver = false;
  gameStarted = false;
  for (int i = 0; i < 3; i++) {
    pipeX[i] = 128 + i * 50;
    gapYArr[i] = random(10, 44);
    pipeDir[i] = random(0, 2) ? 1 : -1;
  }
  coinActive = false;
}

