#include <Adafruit_NeoPixel.h>

#define PIN 6
#define NUMPIXELS 42 // 7 columns x 6 rows
#define COLUMNS 7
#define ROWS 6

Adafruit_NeoPixel strip =
    Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// Button pins
const int buttonPins[COLUMNS] = {2, 3, 4, 5, 6, 7, 8};
const int resetButtonPin = 9;

// Board state
int board[COLUMNS][ROWS];
bool playerTurn = true; // true = player 1 (red), false = player 2 (yellow)

// Colors
uint32_t red = strip.Color(255, 0, 0);
uint32_t yellow = strip.Color(255, 255, 0);
uint32_t empty = strip.Color(0, 0, 0);

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // Initialize buttons
  for (int i = 0; i < COLUMNS; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(resetButtonPin, INPUT_PULLUP);

  resetBoard();
}

void loop() {
  // Check if any column button is pressed
  for (int i = 0; i < COLUMNS; i++) {
    if (digitalRead(buttonPins[i]) == LOW) {
      dropBall(i);
      delay(200); // Debounce delay
    }
  }

  // Check if reset button is pressed
  if (digitalRead(resetButtonPin) == LOW) {
    resetBoard();
    delay(200); // Debounce delay
  }
}

void dropBall(int column) {
  for (int row = ROWS - 1; row >= 0; row--) {
    if (board[column][row] == 0) {
      board[column][row] = playerTurn ? 1 : 2;
      animateDrop(column, row);
      if (checkWin(column, row)) {
        winAnimation();
        delay(3000); // Wait a bit before resetting
        resetBoard();
      } else {
        playerTurn = !playerTurn;
      }
      return;
    }
  }
}

void animateDrop(int column, int row) {
  for (int r = 0; r <= row; r++) {
    strip.setPixelColor(getPixelIndex(column, r), playerTurn ? red : yellow);
    strip.show();
    delay(100); // Adjust for desired drop speed
    if (r < row) {
      strip.setPixelColor(getPixelIndex(column, r), empty);
      strip.show();
    }
  }
}

int getPixelIndex(int column, int row) { return column + (row * COLUMNS); }

void resetBoard() {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, empty);
  }
  strip.show();

  for (int c = 0; c < COLUMNS; c++) {
    for (int r = 0; r < ROWS; r++) {
      board[c][r] = 0;
    }
  }

  playerTurn = true;
}

bool checkWin(int column, int row) {
  int color = board[column][row];
  return checkDirection(column, row, 1, 0, color) +
                 checkDirection(column, row, -1, 0, color) >
             2 ||
         checkDirection(column, row, 0, 1, color) +
                 checkDirection(column, row, 0, -1, color) >
             2 ||
         checkDirection(column, row, 1, 1, color) +
                 checkDirection(column, row, -1, -1, color) >
             2 ||
         checkDirection(column, row, 1, -1, color) +
                 checkDirection(column, row, -1, 1, color) >
             2;
}

int checkDirection(int column, int row, int dColumn, int dRow, int color) {
  int count = 0;
  for (int i = 1; i < 4; i++) {
    int c = column + i * dColumn;
    int r = row + i * dRow;
    if (c >= 0 && c < COLUMNS && r >= 0 && r < ROWS && board[c][r] == color) {
      count++;
    } else {
      break;
    }
  }
  return count;
}

void winAnimation() {
  uint32_t color = playerTurn ? red : yellow;
  for (int i = 0; i < 5; i++) {
    for (int j = 0; j < NUMPIXELS; j++) {
      strip.setPixelColor(j, color);
    }
    strip.show();
    delay(200);
    for (int j = 0; j < NUMPIXELS; j++) {
      strip.setPixelColor(j, empty);
    }
    strip.show();
    delay(200);
  }
}
