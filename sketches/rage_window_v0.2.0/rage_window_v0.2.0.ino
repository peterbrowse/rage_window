/*
 * W+K Rage Window
 * 
 * Arudino sketch to control the rage window vending machine.
 * 
 * v0.2.0
 * 
 * Keypad
 * 
 * Pins setup 1-8 from topside left to right:
 * 
 * COLS = 1, 2, 3, 4
 * ROWS = 5, 6, 7, 8
 * 
 * 1-5 | 2-5 | 3-5 | 4-5
 * 1-6 | 2-6 | 3-6 | 4-6
 * 1-7 | 2-7 | 3-7 | 4-7
 * 1-8 | 2-8 | 3-8 | 4-8
 * 
 * Screen
 * 
 * RS (4) -> 22 / 12 (on diagram)
 * E (6) -> 23 / 11 (on diagram)
 * D4 (11) -> 24 / 5 (on diagram)
 * D5 (12) -> 25 / 4 (on diagram)
 * D6 (13) -> 26 / 3 (on diagram)
 * D7 (14) -> 27 / 2 (on diagram)
 * 
 */

#include <Keypad.h>
#include <LiquidCrystal.h>
#include "Arduino.h"
#include <EDB.h>
#include <EEPROM.h>
#include "TimerObject.h"

#define TABLE_SIZE 512
#define RECORDS_TO_CREATE 6

boolean sold_out = false;

LiquidCrystal lcd(22, 23, 24, 25, 26, 27);

struct itemSelect {
  char id[3];
  int pin;
  boolean active;
} 
itemSelect;

void writer(unsigned long address, byte data) {
  digitalWrite(13, HIGH);
  EEPROM.write(address, data);
  digitalWrite(13, LOW);
}

byte reader(unsigned long address) {
  digitalWrite(13, HIGH);
  return EEPROM.read(address);
  digitalWrite(13, LOW);
}

EDB db(&writer, &reader);

const byte ROWS = 4;
const byte COLS = 4;
unsigned int i = 0;
const int screenPower = 36;

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}  
};

byte rowPins[ROWS] = {3, 4, 5, 6};
byte colPins[COLS] = {7, 8, 9, 10};

char *messages[]={
  "Pick an item...",
  "'*' to confirm.",
  "Cancelled...",
  "Releasing the",
  "Sold out.",
  "Not available.",
  "'#' to cancel.",
  "Insert coins to",
  "Welcome",
  "rage. Enjoy...",
  "Thank you for",
  "your donation.",
  "All sold out."
};

String selection;
 
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);

//TIMERS
TimerObject *timerStart = new TimerObject(5000, &PrintStart, true);
TimerObject *timerActivity1 = new TimerObject(3000, &PrintActivity1, true);
TimerObject *timerActivity2 = new TimerObject(3000, &PrintActivity2, true);
TimerObject *timerActivity3 = new TimerObject(3000, &PrintActivity3, true);
TimerObject *timerCancelled = new TimerObject(1500, &PrintCancelled, true);
TimerObject *timerWelcome = new TimerObject(9000, &PrintWelcome);
TimerObject *timerStartIntro = new TimerObject(6000, &PrintStartIntro);
TimerObject *timerStartCoinMessage = new TimerObject(3000, &PrintCoinMessage, true);

void updateTimers() {
  timerStart->Update();
  timerActivity1->Update();
  timerActivity2->Update();
  timerActivity3->Update();
  timerCancelled->Update();
  timerWelcome->Update();
  timerStartIntro->Update();
  timerStartCoinMessage->Update();
}

void stopTimers() {
  timerStart->Stop();
  timerActivity1->Stop();
  timerActivity2->Stop();
  timerActivity3->Stop();
  timerCancelled->Stop();
  timerWelcome->Stop();
  timerStartIntro->Stop();
  timerStartCoinMessage->Stop();
}

int hammer_falls = 0;
int hammer_falls_maximum = 6;                

void setup() {
  Serial.begin(9600);

  pinMode(screenPower, OUTPUT);
  digitalWrite(screenPower, 0);

  delay(1500);
  
  lcd.begin(16, 2);
  Serial.print("Activating Rage Vending Machine...");
  lcd.print("Starting Rage...");
  Serial.println();

  db.create(0, TABLE_SIZE, sizeof(itemSelect));

  //A1
  sprintf(itemSelect.id, "A1", i++);
  itemSelect.pin = 30;
  pinMode(itemSelect.pin, OUTPUT);
  digitalWrite(itemSelect.pin, 1); 
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //A2
  sprintf(itemSelect.id, "A2", i++);
  itemSelect.pin = 31;
  pinMode(itemSelect.pin, OUTPUT);
  digitalWrite(itemSelect.pin, 1); 
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //A3
  sprintf(itemSelect.id, "A3", i++); 
  itemSelect.pin = 32;
  pinMode(itemSelect.pin, OUTPUT);
  digitalWrite(itemSelect.pin, 1); 
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //A4
  sprintf(itemSelect.id, "A4", i++); 
  itemSelect.pin = 33;
  pinMode(itemSelect.pin, OUTPUT);
  digitalWrite(itemSelect.pin, 1); 
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //A5
  sprintf(itemSelect.id, "A5", i++); 
  itemSelect.pin = 34;
  pinMode(itemSelect.pin, OUTPUT);
  digitalWrite(itemSelect.pin, 1); 
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //A6
  sprintf(itemSelect.id, "A6", i++); 
  itemSelect.pin = 35;
  pinMode(itemSelect.pin, OUTPUT); 
  digitalWrite(itemSelect.pin, 1);
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  delay(2000);
  Serial.print("Item Count: ");
  Serial.println(db.count());
  lcd.clear();
  lcd.print("Items: " + String(db.count()));
  delay(2000);
  lcd.clear();
  PrintCoinInserted();
  delay(1000);
}

void loop() {
  
  char key = customKeypad.getKey();

  if(key) {
    switch (key) {
      case '*':
        stopTimers();
        boolean result;
        boolean activated;
        result = check_result();
        if (result) {
          activated = smash_plate();
          if(activated) {
            release_hammer();
            selection = {""};
            delay(500);
            digitalWrite(screenPower, 1);
            delay(200);
            if(sold_out == false) {
              digitalWrite(screenPower, 0);
              delay(2000);
              lcd.begin(16, 2);
              PrintCoinInserted();
            } else {
              stopTimers();
              digitalWrite(screenPower, 0);
              delay(500);
              lcd.begin(16, 2);
              WriteMessage(messages[12], "");
              Serial.println(messages[12]);
            }
          } else {
            selection = {""};
            Serial.println(messages[4]);
            WriteMessage(messages[4], "");
            timerCancelled->Start();
            timerWelcome->Start();
          }
        } else {
          selection = {""};
          Serial.println(messages[5]);
          WriteMessage(messages[5], "");
          timerCancelled->Start();
          timerWelcome->Start();
        }
        break;
      case '#':
        stopTimers();
        selection = {""};
        Serial.println(messages[2]);
        WriteMessage(messages[2], "");
        timerCancelled->Start();
        timerWelcome->Start();
        break;
      default:
        stopTimers();
        String theKey = String(key);
        selection = selection + theKey;
        Serial.println(selection);
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(selection);
      }
   }

   updateTimers();
}

void StartIntro() {
  WriteMessage(messages[7], messages[0]);
  timerWelcome->Start();
}

void PrintStartIntro() {
  PrintStart();
  timerStartCoinMessage->Start();
}

void PrintIntro() {
  WriteMessage(messages[8], "");
  timerStart->Start();
}

void PrintStart() {
  WriteMessage(messages[8], "");
}

void PrintCoinMessage() {
  WriteMessage(messages[7], messages[0]);
}

void PrintCoinInserted() {
  WriteMessage(messages[0], "");
  timerActivity1->Start();
}

void PrintActivity1() {
  WriteMessage(messages[1], "");
  timerActivity2->Start();
}

void PrintActivity2() {
  WriteMessage(messages[6], "");
  timerActivity3->Start();
}

void PrintActivity3() {
  WriteMessage(messages[0], "");
}

void PrintCancelled() {
  PrintCoinInserted();
}

void PrintWelcome() {
  PrintCoinInserted();
}

void WriteMessage(String message_line_one, String message_line_two) {
  lcd.clear();
  if(message_line_one) {
    lcd.print(message_line_one);
  }

  if(message_line_two) {
    lcd.setCursor(0,1);
    lcd.print(message_line_two);
  }
}

char* string2char(String command){
    if(command.length()!=0){
        char *p = const_cast<char*>(command.c_str());
        return p;
    }
}

boolean smash_plate() {
  boolean smash_result = false;

  for(int plates = 1; plates <= db.count(); plates++) {
    EDB_Status result = db.readRec(plates, EDB_REC itemSelect);
    if (result == EDB_OK) {
      if(String(itemSelect.id) == selection) {
        if(itemSelect.active == true) {
          smash_result = true;
          itemSelect.active = false;
          EDB_Status result = db.updateRec(plates, EDB_REC itemSelect);
          if (result != EDB_OK) printError(result);
        }
      }
    }
  }

  return smash_result;
}

boolean check_result() {
  boolean check_result = false;
  
  for (int plates = 1; plates <= db.count(); plates++) {
    EDB_Status result = db.readRec(plates, EDB_REC itemSelect);
    if (result == EDB_OK) {
      if(String(itemSelect.id) == selection) {
        check_result = true;
      }
    } else printError(result);
  }

  return check_result;
}

void release_hammer() {
  for(int plates = 1; plates <= db.count(); plates++) {
    EDB_Status result = db.readRec(plates, EDB_REC itemSelect);
    if (result == EDB_OK) {
      if(String(itemSelect.id) == selection) {
        digitalWrite(itemSelect.pin, 0);
        delay(200);
        digitalWrite(itemSelect.pin, 1);
        Serial.println(itemSelect.pin);

        hammer_falls++;
        Serial.print("Hammer count: ");
        Serial.println(hammer_falls);
        if(hammer_falls >= hammer_falls_maximum) {
          sold_out = true;
        }
      }
    } else printError(result);
  }
}

void printError(EDB_Status err) {
    Serial.print("ERROR: ");
    switch (err) {
        case EDB_OUT_OF_RANGE:
            Serial.println("Items out of range");
            break;
        case EDB_TABLE_FULL:
            Serial.println("Table full");
            break;
        case EDB_OK:
        default:
            Serial.println("OK");
            break;
    }
}

void selectAll()
{
    for (int plates = 1; plates <= db.count(); plates++)
    {
        EDB_Status result = db.readRec(plates, EDB_REC itemSelect);
        if (result == EDB_OK)
        {
            Serial.print("Items: ");
            Serial.print(plates);
            Serial.print(" ID: ");
            Serial.print(itemSelect.id);
            Serial.print(" Active: ");
            Serial.println(itemSelect.active);
        }
        else printError(result);
    }
}
