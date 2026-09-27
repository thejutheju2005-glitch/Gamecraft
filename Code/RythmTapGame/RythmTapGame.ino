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

const int buttonPin = 2;

// --- Track / Hit zone config ---
const int trackX = 60;
const int trackWidth = 20;
const int hitZoneY = 50;      // where notes should be hit
const int hitZoneHeight = 6;
const int spawnY = 0;

// --- Note system ---
#define MAX_NOTES 6
float noteY[MAX_NOTES];       // use float for smoother sub-pixel speed control
bool noteActive[MAX_NOTES];

float noteSpeed = 1.2;        // pixels per frame
unsigned long lastSpawnTime = 0;
int spawnInterval = 1200;     // ms between notes, decreases with level

// --- Scoring ---
int score = 0;
int level = 1;
int combo = 0;
int maxCombo = 0;
int misses = 0;
int maxMisses = 5;
int bestScore = 0;

// Feedback text display
String feedbackText = "";
unsigned long feedbackTime = 0;
const int feedbackDuration = 400;

bool lastButtonState = HIGH;

enum GameState { START_SCREEN, PLAYING, GAME_OVER_STATE };
GameState state = START_SCREEN;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  display.begin(0, true);
  display.clearDisplay();
  display.display();

  randomSeed(analogRead(0));
  resetNotes();
}

void resetNotes() {
  for (int i = 0; i < MAX_NOTES; i++) {
    noteActive[i] = false;
    noteY[i] = spawnY;
  }
}

void loop() {
  switch (state) {
    case START_SCREEN:
      drawStartScreen();
      if (digitalRead(buttonPin) == HIGH) {
        delay(200);
        startGame();
      }
      break;

    case PLAYING:
      updateGame();
      drawGame();
      delay(20);
      break;

    case GAME_OVER_STATE:
      drawGameOver();
      if (digitalRead(buttonPin) == HIGH) {
        delay(200);
        state = START_SCREEN;
      }
      break;
  }
}

void startGame() {
  score = 0;
  level = 1;
  combo = 0;
  maxCombo = 0;
  misses = 0;
  noteSpeed = 1.2;
  spawnInterval = 1200;
  resetNotes();
  lastSpawnTime = millis();
  feedbackText = "";
  state = PLAYING;
}

void spawnNote() {
  for (int i = 0; i < MAX_NOTES; i++) {
    if (!noteActive[i]) {
      noteActive[i] = true;
      noteY[i] = spawnY;
      break;
    }
  }
}

void updateGame() {
  unsigned long now = millis();

  // Spawn new notes at interval
  if (now - lastSpawnTime > spawnInterval) {
    spawnNote();
    lastSpawnTime = now;
  }

  // Move notes down, check for missed notes
  for (int i = 0; i < MAX_NOTES; i++) {
    if (noteActive[i]) {
      noteY[i] += noteSpeed;

      // Missed the hit zone completely (went past bottom)
      if (noteY[i] > 64) {
        noteActive[i] = false;
        registerMiss();
      }
    }
  }

  // Button press detection (edge-triggered)
  bool currentButtonState = digitalRead(buttonPin);
  if (currentButtonState == HIGH && lastButtonState == LOW) {
    handleTap();
  }
  lastButtonState = currentButtonState;

  // Clear feedback text after duration
  if (feedbackText != "" && now - feedbackTime > feedbackDuration) {
    feedbackText = "";
  }

  // Check game over condition
  if (misses >= maxMisses) {
    if (score > bestScore) bestScore = score;
    state = GAME_OVER_STATE;
  }
}

void handleTap() {
  // Find the note closest to the hit zone (within a reasonable window)
  int closestIndex = -1;
  float closestDist = 999;

  for (int i = 0; i < MAX_NOTES; i++) {
    if (noteActive[i]) {
      float dist = abs(noteY[i] - hitZoneY);
      if (dist < closestDist) {
        closestDist = dist;
        closestIndex = i;
      }
    }
  }

  if (closestIndex == -1) {
    // No notes nearby at all - false tap
    feedbackText = "EARLY!";
    feedbackTime = millis();
    return;
  }

  // Scoring based on distance from perfect hit
  if (closestDist <= 3) {
    // PERFECT
    score += 10;
    combo++;
    feedbackText = "PERFECT!";
  } else if (closestDist <= 8) {
    // GOOD
    score += 5;
    combo++;
    feedbackText = "GOOD";
  } else {
    // Too far - counts as miss/false tap
    registerMiss();
    feedbackText = "MISS";
    feedbackTime = millis();
    return;
  }

  feedbackTime = millis();
  noteActive[closestIndex] = false;

  if (combo > maxCombo) maxCombo = combo;

  // Level up every 8 combo points
  if (combo > 0 && combo % 8 == 0) {
    level++;
    noteSpeed += 0.3;
    if (spawnInterval > 500) spawnInterval -= 100;
  }
}

void registerMiss() {
  misses++;
  combo = 0;
  feedbackText = "MISS";
  feedbackTime = millis();
}

// --- DRAWING ---

void drawGame() {
  display.clearDisplay();

  // Draw track boundaries
  display.drawLine(trackX, 0, trackX, 64, SH110X_WHITE);
  display.drawLine(trackX + trackWidth, 0, trackX + trackWidth, 64, SH110X_WHITE);

  // Draw hit zone
  display.drawRect(trackX, hitZoneY, trackWidth, hitZoneHeight, SH110X_WHITE);

  // Draw notes
  for (int i = 0; i < MAX_NOTES; i++) {
    if (noteActive[i]) {
      display.fillRect(trackX + 2, (int)noteY[i], trackWidth - 4, 5, SH110X_WHITE);
    }
  }

  // Draw feedback text
  if (feedbackText != "") {
    display.setTextSize(1);
    display.setTextColor(SH110X_WHITE);
    display.setCursor(trackX - 15, hitZoneY - 20);
    display.print(feedbackText);
  }

  // HUD - left side: score & level
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.print("Score:");
  display.setCursor(0, 10);
  display.print(score);

  display.setCursor(0, 25);
  display.print("Lvl:");
  display.print(level);

  // HUD - right side: combo & misses
  display.setCursor(95, 0);
  display.print("Combo");
  display.setCursor(100, 10);
  display.print(combo);

  display.setCursor(90, 50);
  display.print("Miss:");
  display.print(misses);
  display.print("/");
  display.print(maxMisses);

  display.display();
}

void drawStartScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(30, 8);
  display.print("RHYTHM TAP");
  display.setCursor(5, 22);
  display.print("Tap when note hits");
  display.setCursor(5, 32);
  display.print("the box at bottom!");
  display.setCursor(5, 46);
  display.print("Perfect timing = more");
  display.setCursor(5, 54);
  display.print("points. Good luck!");
  display.display();
  delay(50); // small stability delay, non-blocking enough for start screen
}

void drawGameOver() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(30, 8);
  display.print("GAME OVER");
  display.setCursor(15, 22);
  display.print("Score: ");
  display.print(score);
  display.setCursor(15, 33);
  display.print("Max Combo: ");
  display.print(maxCombo);
  display.setCursor(15, 44);
  display.print("Best Score: ");
  display.print(bestScore);
  display.setCursor(5, 56);
  display.print("Press to Play Again");
  display.display();
}
