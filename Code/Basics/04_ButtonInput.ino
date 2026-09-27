/*
  GameCraft - Lesson 4: Button Input
  -------------------------------------
  Goal: Learn how to read button presses reliably.
  
  Concepts covered:
  - digitalRead() and pull-up resistors
  - The difference between "is pressed" vs "was just pressed" (edge detection)
  - Debouncing (avoiding false multiple triggers)
  - Measuring press duration (short vs long press)
  
  🎯 This is the FINAL piece before you can build a complete game!
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

// Variables for edge detection
bool lastButtonState = HIGH;   // HIGH = not pressed (because of INPUT_PULLUP)
int pressCount = 0;

// Variables for press duration measurement
unsigned long pressStartTime = 0;
bool isCurrentlyPressed = false;
String lastPressType = "None";

void setup() {
  // INPUT_PULLUP means: button reads HIGH normally, 
  // and LOW when pressed (button connects pin to GND)
  pinMode(buttonPin, INPUT_PULLUP);

  display.begin(0, true);
  display.clearDisplay();
  display.display();
}

void loop() {
  display.clearDisplay();

  // =====================================================
  // PART 1: Simple read - "is the button pressed RIGHT NOW"
  // =====================================================
  // NOTE: Because of INPUT_PULLUP wiring convention used in 
  // your earlier games, HIGH = pressed (button pulls pin to 
  // a different state). Check your specific button wiring - 
  // some setups read LOW when pressed instead!
  bool currentButtonState = digitalRead(buttonPin);

  // =====================================================
  // PART 2: Edge Detection - "was the button JUST pressed"
  // This fires ONCE per press, not repeatedly while held.
  // This is what we used in Memory Beep and Rhythm Tap!
  // =====================================================
  bool justPressed = (currentButtonState == HIGH && lastButtonState == LOW);
  bool justReleased = (currentButtonState == LOW && lastButtonState == HIGH);

  if (justPressed) {
    pressCount++;
    pressStartTime = millis();
    isCurrentlyPressed = true;
  }

  if (justReleased) {
    unsigned long duration = millis() - pressStartTime;
    isCurrentlyPressed = false;

    // ===================================================
    // PART 3: Measuring press duration - short vs long
    // This is exactly how Memory Beep detected SHORT/LONG!
    // ===================================================
    if (duration < 400) {
      lastPressType = "SHORT (" + String(duration) + "ms)";
    } else {
      lastPressType = "LONG (" + String(duration) + "ms)";
    }
  }

  lastButtonState = currentButtonState; // save for next loop comparison

  // =====================================================
  // DISPLAY everything so we can SEE what's happening
  // =====================================================
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);

  display.setCursor(0, 0);
  display.print("Raw state: ");
  display.print(currentButtonState == HIGH ? "PRESSED" : "released");

  display.setCursor(0, 12);
  display.print("Press count: ");
  display.print(pressCount);

  display.setCursor(0, 24);
  display.print("Last type: ");
  display.setCursor(0, 34);
  display.print(lastPressType);

  // Visual indicator - a circle that fills when pressed
  display.drawCircle(110, 45, 10, SH110X_WHITE);
  if (isCurrentlyPressed) {
    display.fillCircle(110, 45, 7, SH110X_WHITE);
  }

  // Show live hold duration while pressed (bar grows)
  if (isCurrentlyPressed) {
    unsigned long heldTime = millis() - pressStartTime;
    int barWidth = constrain(map(heldTime, 0, 800, 0, 100), 0, 100);
    display.drawRect(10, 55, 100, 8, SH110X_WHITE);
    display.fillRect(10, 55, barWidth, 8, SH110X_WHITE);
  }

  display.display();

  delay(20); // small delay for stability - not strictly needed but keeps loop from running too fast
}

/*
  🧪 TRY THIS YOURSELF:
  1. Tap the button quickly - watch "Press count" go up by exactly 1
     per tap, NOT multiple times (that's edge detection working!).
  
  2. Try removing the "justPressed" check and instead just do:
     "if (currentButtonState == HIGH) { pressCount++; }"
     - You'll see the count skyrockets because it counts EVERY 
       loop cycle while held, not just once per press!
       This demonstrates WHY edge detection matters.
  
  3. Hold the button for different durations and watch it correctly 
     classify SHORT vs LONG press - this is the exact logic used 
     in the Memory Beep game!
  
  4. Try changing the 400ms threshold to 200ms or 800ms - see how 
     it changes what counts as "long".

  📌 KEY INSIGHT - CONNECT TO REAL GAMES:
  - Flappy Bird & Dino Runner: use SIMPLE read (Part 1) - checking 
    "is button pressed" every frame is fine when you want continuous 
    action (like flying up).
  
  - Rhythm Tap: uses EDGE DETECTION (Part 2) - needs to know exactly 
    once when you tap, not repeatedly.
  
  - Memory Beep: uses DURATION MEASUREMENT (Part 3) - needs to know 
    HOW LONG you held the button to distinguish short vs long press.
  
  You now understand ALL the input techniques used across every 
  GameCraft game!
*/
