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

// Game state machine
enum GameState { START_SCREEN, SHOWING_SEQUENCE, WAITING_INPUT, ROUND_SUCCESS, ROUND_FAIL, GAME_OVER_STATE };
GameState state = START_SCREEN;

#define MAX_SEQUENCE 50
int sequence[MAX_SEQUENCE]; // 0 = short, 1 = long
int sequenceLength = 1;
int showIndex = 0;
int inputIndex = 0;

int score = 0;
int level = 1;
int bestScore = 0;

// Timing constants
const int SHORT_DURATION = 300;   // ms - how long a "short" flash shows
const int LONG_DURATION = 700;    // ms - how long a "long" flash shows
const int GAP_DURATION = 300;     // ms - gap between flashes
const int LONG_PRESS_THRESHOLD = 400; // ms - press longer than this = "long"

unsigned long stateTimer = 0;
bool flashOn = false;

// Button press tracking
bool buttonWasPressed = false;
unsigned long pressStartTime = 0;

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  display.begin(0, true);
  display.clearDisplay();
  display.display();

  randomSeed(analogRead(0));
  generateSequence();
}

void generateSequence() {
  for (int i = 0; i < MAX_SEQUENCE; i++) {
    sequence[i] = random(0, 2); // 0 or 1
  }
}

void loop() {
  switch (state) {
    case START_SCREEN:
      drawStartScreen();
      if (digitalRead(buttonPin) == HIGH) {
        delay(200);
        startNewGame();
      }
      break;

    case SHOWING_SEQUENCE:
      handleShowingSequence();
      break;

    case WAITING_INPUT:
      handleWaitingInput();
      break;

    case ROUND_SUCCESS:
      drawRoundSuccess();
      if (millis() - stateTimer > 1000) {
        sequenceLength++;
        score += level;
        if (sequenceLength % 3 == 0) level++;
        showIndex = 0;
        inputIndex = 0;
        state = SHOWING_SEQUENCE;
        stateTimer = millis();
        flashOn = false;
      }
      break;

    case ROUND_FAIL:
      drawRoundFail();
      if (millis() - stateTimer > 1500) {
        state = GAME_OVER_STATE;
      }
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

void startNewGame() {
  sequenceLength = 1;
  score = 0;
  level = 1;
  showIndex = 0;
  inputIndex = 0;
  generateSequence();
  state = SHOWING_SEQUENCE;
  stateTimer = millis();
  flashOn = false;
}

// --- SHOWING SEQUENCE STATE ---
void handleShowingSequence() {
  unsigned long elapsed = millis() - stateTimer;
  int currentDuration = sequence[showIndex] == 1 ? LONG_DURATION : SHORT_DURATION;

  if (!flashOn) {
    // Start flash
    flashOn = true;
    stateTimer = millis();
    drawFlash(sequence[showIndex], true);
  } else {
    if (elapsed < currentDuration) {
      // Keep showing flash (avoid redrawing every loop, but fine for OLED)
      drawFlash(sequence[showIndex], true);
    } else if (elapsed < currentDuration + GAP_DURATION) {
      // Gap - blank screen
      drawFlash(sequence[showIndex], false);
    } else {
      // Move to next flash
      showIndex++;
      flashOn = false;
      stateTimer = millis();
      if (showIndex >= sequenceLength) {
        // Done showing, wait for input
        inputIndex = 0;
        state = WAITING_INPUT;
        drawWaitingScreen();
      }
    }
  }
}

// --- WAITING FOR PLAYER INPUT ---
void handleWaitingInput() {
  bool currentlyPressed = (digitalRead(buttonPin) == HIGH);

  if (currentlyPressed && !buttonWasPressed) {
    // Button just pressed down
    pressStartTime = millis();
    buttonWasPressed = true;
  }

  if (!currentlyPressed && buttonWasPressed) {
    // Button just released - measure press duration
    unsigned long pressDuration = millis() - pressStartTime;
    buttonWasPressed = false;

    int playerInput = (pressDuration >= LONG_PRESS_THRESHOLD) ? 1 : 0;

    if (playerInput == sequence[inputIndex]) {
      // Correct!
      inputIndex++;
      drawCorrectFeedback();
      if (inputIndex >= sequenceLength) {
        // Completed full sequence correctly
        state = ROUND_SUCCESS;
        stateTimer = millis();
      }
    } else {
      // Wrong!
      if (score > bestScore) bestScore = score;
      state = ROUND_FAIL;
      stateTimer = millis();
    }
  }

  // Show live feedback while holding button (growing bar)
  if (currentlyPressed) {
    unsigned long heldTime = millis() - pressStartTime;
    drawHoldingFeedback(heldTime);
  }
}

// --- DRAWING FUNCTIONS ---

void drawFlash(int type, bool on) {
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.print("Level:");
  display.print(level);
  display.setCursor(80, 0);
  display.print("Seq:");
  display.print(sequenceLength);

  display.setCursor(10, 15);
  display.print("WATCH CAREFULLY");

  if (on) {
    if (type == 1) {
      // LONG flash - wide bar
      display.fillRoundRect(14, 30, 100, 20, 3, SH110X_WHITE);
      display.setTextColor(SH110X_BLACK);
      display.setCursor(45, 37);
      display.print("LONG");
    } else {
      // SHORT flash - narrow bar
      display.fillRoundRect(44, 30, 40, 20, 3, SH110X_WHITE);
      display.setTextColor(SH110X_BLACK);
      display.setCursor(50, 37);
      display.print("SHORT");
    }
  }

  display.display();
}

void drawWaitingScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 0);
  display.print("Level:");
  display.print(level);
  display.setCursor(80, 0);
  display.print("Seq:");
  display.print(sequenceLength);

  display.setCursor(15, 20);
  display.print("YOUR TURN!");
  display.setCursor(5, 35);
  display.print("Short tap / Long hold");
  display.setCursor(20, 48);
  display.print("Step: ");
  display.print(inputIndex + 1);
  display.print("/");
  display.print(sequenceLength);

  display.display();
}

void drawHoldingFeedback(unsigned long heldTime) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(20, 10);
  display.print("HOLDING...");

  // Growing bar as feedback - fills up as you hold
  int barWidth = map(constrain(heldTime, 0, LONG_PRESS_THRESHOLD + 200), 0, LONG_PRESS_THRESHOLD + 200, 0, 100);
  display.drawRect(14, 30, 100, 20, SH110X_WHITE);
  display.fillRect(14, 30, barWidth, 20, SH110X_WHITE);

  // Marker line showing long-press threshold
  int thresholdX = 14 + map(LONG_PRESS_THRESHOLD, 0, LONG_PRESS_THRESHOLD + 200, 0, 100);
  display.drawLine(thresholdX, 25, thresholdX, 55, SH110X_WHITE);

  display.display();
}

void drawCorrectFeedback() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(30, 25);
  display.print("GOOD!");
  display.display();
  delay(300); // brief pause for feedback
}

void drawRoundSuccess() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(15, 15);
  display.print("SEQUENCE COMPLETE!");
  display.setCursor(30, 35);
  display.print("Score: ");
  display.print(score);
  display.setCursor(20, 48);
  display.print("Next round...");
  display.display();
}

void drawRoundFail() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(30, 15);
  display.print("WRONG!");
  display.setCursor(15, 30);
  display.print("Expected: ");
  display.print(sequence[inputIndex] == 1 ? "LONG" : "SHORT");
  display.setCursor(20, 45);
  display.print("Score: ");
  display.print(score);
  display.display();
}

void drawGameOver() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(30, 10);
  display.print("GAME OVER");
  display.setCursor(20, 25);
  display.print("Final Score: ");
  display.print(score);
  display.setCursor(20, 38);
  display.print("Best Score: ");
  display.print(bestScore);
  display.setCursor(10, 52);
  display.print("Press to Play Again");
  display.display();
}

void drawStartScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(25, 10);
  display.print("MEMORY BEEP");
  display.setCursor(5, 25);
  display.print("Watch the pattern,");
  display.setCursor(5, 35);
  display.print("then repeat it back!");
  display.setCursor(5, 48);
  display.print("Short tap / Long hold");
  display.setCursor(35, 58);
  display.print("Press to Start");
  display.display();
}
