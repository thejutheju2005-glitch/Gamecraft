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
int barX = 10;
int barDir = 1;
int barSpeed = 2;

int zoneStart = 50;
int zoneWidth = 25;

int score = 0;
int level = 1;
int lives = 3;

bool roundActive = true;   // bar is moving, waiting for press
bool showResult = false;
bool hitSuccess = false;
unsigned long resultTime = 0;

bool gameOver = false;
bool gameStarted = false;

const int buttonPin = 2;
bool lastButtonState = HIGH;

// Track combo streak
int streak = 0;
int bestStreak = 0;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  display.begin(0, true);
  display.clearDisplay();
  display.display();

  randomSeed(analogRead(0));
  setupRound();
}

void setupRound() {
  barX = 10;
  barDir = 1;
  // Randomize target zone position a bit each round
  zoneStart = random(20, 128 - zoneWidth - 20);
  roundActive = true;
  showResult = false;
}

void loop() {
  if (!gameStarted) {
    drawStartScreen();
    if (digitalRead(buttonPin) == HIGH) {
      gameStarted = true;
      delay(200);
    }
  } else if (!gameOver) {
    updateGame();
    drawGame();
    delay(20);
  } else {
    drawGameOver();
    if (digitalRead(buttonPin) == HIGH) {
      resetGame();
    }
  }
}

void updateGame() {
  bool currentButtonState = digitalRead(buttonPin);

  if (roundActive) {
    // Move bar back and forth
    barX += barDir * barSpeed;
    if (barX >= 128 - 4) { barX = 128 - 4; barDir = -1; }
    if (barX <= 0) { barX = 0; barDir = 1; }

    // Detect button press (edge detect so holding doesn't spam)
    if (currentButtonState == HIGH && lastButtonState == LOW) {
      // Player pressed - check if bar is inside zone
      checkResult();
    }
  } else if (showResult) {
    // Show result briefly then move to next round
    if (millis() - resultTime > 800) {
      if (lives <= 0) {
        gameOver = true;
      } else {
        setupRound();
      }
    }
  }

  lastButtonState = currentButtonState;
}

void checkResult() {
  int barCenter = barX + 2;
  if (barCenter >= zoneStart && barCenter <= zoneStart + zoneWidth) {
    // Success
    hitSuccess = true;
    streak++;
    if (streak > bestStreak) bestStreak = streak;

    // Score based on how centered the hit was
    int zoneCenter = zoneStart + zoneWidth / 2;
    int distFromCenter = abs(barCenter - zoneCenter);
    int points = max(1, 10 - distFromCenter);
    score += points;

    // Level up every 5 successful hits
    if (streak % 5 == 0) {
      level++;
      barSpeed++;
      if (zoneWidth > 10) zoneWidth -= 2; // harder: smaller zone
    }
  } else {
    // Miss
    hitSuccess = false;
    streak = 0;
    lives--;
  }

  roundActive = false;
  showResult = true;
  resultTime = millis();
}

void drawGame() {
  display.clearDisplay();

  // Draw track
  display.drawRect(0, 30, 128, 8, SH110X_WHITE);

  // Draw target zone
  display.fillRect(zoneStart, 30, zoneWidth, 8, SH110X_WHITE);
  // Outline it differently by drawing inverse edges (hollow look with border)
  display.drawRect(zoneStart, 30, zoneWidth, 8, SH110X_WHITE);

  if (roundActive) {
    // Draw moving bar (as a marker on top of track)
    display.fillRect(barX, 28, 4, 12, SH110X_WHITE);
  }

  // Show result feedback
  if (showResult) {
    display.setTextSize(1);
    display.setCursor(35, 15);
    if (hitSuccess) {
      display.print("NICE HIT!");
    } else {
      display.print("MISSED!");
    }
  }

  // HUD - Score, Level, Lives, Streak
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("S:");
  display.print(score);

  display.setCursor(40, 0);
  display.print("L:");
  display.print(level);

  display.setCursor(80, 0);
  display.print("Lives:");
  display.print(lives);

  display.setCursor(0, 55);
  display.print("Streak:");
  display.print(streak);

  display.setCursor(75, 55);
  display.print("Best:");
  display.print(bestStreak);

  display.display();
}

void drawStartScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(20, 15);
  display.print("STOP THE BAR");
  display.setCursor(10, 30);
  display.print("Press when bar is");
  display.setCursor(25, 40);
  display.print("in the zone!");
  display.setCursor(45, 52);
  display.print("Start");
  display.display();
}

void drawGameOver() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(30, 10);
  display.print("GAME OVER");
  display.setCursor(20, 25);
  display.print("Score: ");
  display.print(score);
  display.setCursor(20, 38);
  display.print("Best Streak: ");
  display.print(bestStreak);
  display.setCursor(15, 52);
  display.print("Press to Retry");
  display.display();
}

void resetGame() {
  score = 0;
  level = 1;
  lives = 3;
  streak = 0;
  barSpeed = 2;
  zoneWidth = 25;
  gameOver = false;
  gameStarted = false;
  setupRound();
}
