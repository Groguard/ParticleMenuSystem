/*
 * Project MenuSystemTest
 * Description:
 * Author:
 * Date:
 */

#include "Adafruit_ILI9341.h"

// For the Adafruit shield, these are the default.
#define TFT_DC D5
#define TFT_CS D4
#define TFT_MOSI A5
#define TFT_CLK A3
#define TFT_RST D6
#define TFT_MISO A4

#define up D3
#define down D2
#define select D1

#define UP 1
#define DOWN 2
#define SELECT 3

//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
// If using the breakout, change pins as desired
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

const int arraySize = 100;
bool appRunning = false;
int selected; // currently selected menu item
int upButtonState = 0;
int downButtonState = 0;
int selectButtonState = 0;
char* current_menu[arraySize] = {0};
uint8_t current_menu_index = 1; // current position in the array
uint8_t current_menu_size;
bool page_change = false;
uint8_t menu_top = 35; // top space between title and items

typedef void (* CurrentFuncPtr) (); // Function pointer for current menu
typedef void (* MenuFuncPtr) (); // Function pointer for Main Menu
typedef void (* Test1FuncPtr) (); // Function pointer for Test Menu 1
typedef void (* Test2FuncPtr) (); // Function pointer for Test Menu 2

// function prototypes
void setMenu(char** menu, CurrentFuncPtr* funcs, uint8_t size);
void buildTestMenu1(void);
void buildTestMenu2(void);
void returnMainMenu(void);

char* mainMenu[3] = {"    Main Menu",
                    "Test Menu 1   ",
                    "Test Menu 2   "
                   };

char* testMenu1[4] = {"   Test Menu 1",
                    "Option 1   ",
                    "Option 2   ",
                    "Back       "
                   };

char* testMenu2[4] = {"   Test Menu 2",
                    "Option 1   ",
                    "Option 2   ",
                    "Back       "
                   };


CurrentFuncPtr* current_func = NULL;

// Main menu func pointer
MenuFuncPtr menu_func[4] = {0,
                            buildTestMenu1,
                            buildTestMenu2};
// Test menu Func Pointer
Test1FuncPtr test1_func[5] = {0,
                            0,
                            0,
                            returnMainMenu};

Test2FuncPtr test2_func[5] = {0,
                            0,
                            0,
                            returnMainMenu};


void setup() {
  Serial.begin(9600);

  // Button Setup
  pinMode(up, INPUT_PULLUP);

  pinMode(down, INPUT_PULLUP);

  pinMode(select, INPUT_PULLUP);

  tft.begin();
  tft.setTextWrap(false);
  tft.setRotation(3);
  tft.setTextSize(3);
  setMenu(mainMenu, menu_func, sizeof(mainMenu)/sizeof(char *)-1);
  displayMenu();
}

void menuInput(void) {
  // read the state of the pushbutton value:
  upButtonState = digitalRead(up);
  downButtonState = digitalRead(down);
  selectButtonState = digitalRead(select);

  if (upButtonState == LOW) {
    // Up
    action(UP);
  }
  else if (downButtonState == LOW) {
    // Down
    action(DOWN);
  }
  else if (selectButtonState == LOW) {
    action(SELECT);
  }
  delay(100);
}

void loop() {
  while(!appRunning) {
    menuInput();
  }
}

void returnMainMenu(void) {
  setMenu(mainMenu, menu_func, sizeof(mainMenu)/sizeof(char *)-1);
  displayMenu();
}

void buildTestMenu1(void) {
  setMenu(testMenu1, test1_func, sizeof(testMenu1)/sizeof(char *)-1);
  displayMenu();
}

void buildTestMenu2(void) {
  setMenu(testMenu2, test2_func, sizeof(testMenu2)/sizeof(char *)-1);
  displayMenu();
}


void setMenu(char** menu, CurrentFuncPtr* funcs, uint8_t size) {
  memset(current_menu, 0, arraySize);

  current_func = funcs;
  int i;
  for(i=0; i<=size; i++) {
    current_menu[i] = menu[i];
  }
  selected = 1;
  current_menu_size = i-2;
}

void displayMenu(void) {
  int i;
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_GREEN, ILI9341_BLACK);
  tft.println(current_menu[0]);
  tft.drawLine(0, 25, tft.width()-1, 25, ILI9341_GREEN);

  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
  for(i = current_menu_index; i <= (current_menu_index + 6); i++) {
      tft.setCursor(0, ((i - current_menu_index) * 30) + menu_top);
      tft.println(current_menu[i]);
  }
  menuHighlight(selected);
}

void menuHighlight(uint8_t index) {
  if (page_change == false) {
      tft.setCursor(0, ((selected - current_menu_index) * 30) + menu_top);
      tft.setTextColor(ILI9341_YELLOW, ILI9341_BLACK);
      tft.println(current_menu[selected]);
  }
  tft.setCursor(0, ((index - current_menu_index) * 30) + menu_top);
  tft.setTextColor(ILI9341_YELLOW, ILI9341_BLUE);
  tft.println(current_menu[index]);
  selected = index;
  page_change = false;
}

void action(uint8_t action) {
  switch (action) {
      case 1: { // up
          if (selected <= (current_menu_index + 6) && (selected > current_menu_index)) {
              menuHighlight(selected - 1);
          }
          else if ((current_menu_index > 1) && (selected <= current_menu_index)) {
              current_menu_index = (current_menu_index - 1);
              selected = selected - 1;
              page_change = true;
              displayMenu();
          }
          break;
      }
      case 2: { // down
          if (selected >= (current_menu_index + 6) &&
              (current_menu_size > 6) && (selected <= current_menu_size)) {
              current_menu_index = (current_menu_index + 1);
              selected = selected + 1;
              page_change = true;
              displayMenu();
          }
          else if ((selected < (current_menu_index + 6)) &&
                  (selected <= current_menu_size)) {
              menuHighlight(selected + 1);
          }
          break;
      }
      case 3: // select
          current_func[selected]();//
          break;
  }
}
