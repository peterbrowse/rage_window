/*
 * W+K Rage Window
 * 
 * Arudino sketch to control the rage window vending machine.
 * 
 * v0.1.0
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
#define RECORDS_TO_CREATE 9

boolean coin_active = false;

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

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}  
};

byte rowPins[ROWS] = {2, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

char *messages[]={
  "Pick a plate...",
  "'*' to confirm.",
  "Cancelled...",
  "Releasing the",
  "Sold out.",
  "Not available.",
  "'#' to cancel.",
  "Insert coins to",
  "Welcome",
  "rage. Enjoy..."
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

void setup() {
  coin_active = true;
  
  Serial.begin(9600);
  
  lcd.begin(16, 2);
  Serial.print("Activating Rage Vending Machine...");
  lcd.print("Starting Rage...");
  Serial.println();

  db.create(0, TABLE_SIZE, sizeof(itemSelect));

  //A1
  sprintf(itemSelect.id, "A1", i++);
  itemSelect.pin = 30;
  digitalWrite(itemSelect.pin, 0);
  pinMode(itemSelect.pin, OUTPUT); 
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //A2
  sprintf(itemSelect.id, "A2", i++);
  itemSelect.pin = 31;
  digitalWrite(itemSelect.pin, 0);
  pinMode(itemSelect.pin, OUTPUT); 
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //A3
  sprintf(itemSelect.id, "A3", i++); 
  itemSelect.pin = 32;
  digitalWrite(itemSelect.pin, 0);
  pinMode(itemSelect.pin, OUTPUT); 
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //B1
  sprintf(itemSelect.id, "B1", i++); 
  itemSelect.pin = 33;
  digitalWrite(itemSelect.pin, 0);
  pinMode(itemSelect.pin, OUTPUT); 
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //B2
  sprintf(itemSelect.id, "B2", i++); 
  itemSelect.pin = 34;
  digitalWrite(itemSelect.pin, 0);
  pinMode(itemSelect.pin, OUTPUT); 
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //B3
  sprintf(itemSelect.id, "B3", i++); 
  itemSelect.pin = 35;
  digitalWrite(itemSelect.pin, 0);
  pinMode(itemSelect.pin, OUTPUT); 
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //C1
  sprintf(itemSelect.id, "C1", i++); 
  itemSelect.pin = 36;
  digitalWrite(itemSelect.pin, 0);
  pinMode(itemSelect.pin, OUTPUT); 
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //C2
  sprintf(itemSelect.id, "C2", i++); 
  itemSelect.pin = 37;
  digitalWrite(itemSelect.pin, 0);
  pinMode(itemSelect.pin, OUTPUT); 
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //C3
  sprintf(itemSelect.id, "C3", i++);  
  itemSelect.pin = 38;
  digitalWrite(itemSelect.pin, 0);
  pinMode(itemSelect.pin, OUTPUT); 
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  delay(2000);
  Serial.print("Item Count: ");
  Serial.println(db.count());
  lcd.clear();
  lcd.print("Plates: " + String(db.count()));
  delay(2000);
  lcd.clear();
  StartIntro();
}

void loop() {
  
char key = customKeypad.getKey();

  if(key) {
    if(coin_active) {
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
            Serial.println(messages[3]);
            WriteMessage(messages[3], messages[9]);
            delay(4000);
            StartIntro();
            coin_active = false;
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
    } else {
      stopTimers();
      StartIntro();
    }
   }

   updateTimers();
}

void StartIntro() {
  WriteMessage(messages[7], messages[0]);
  timerStartIntro->Start();
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

void CoinInserted () {
  stopTimers();
  PrintCoinInserted();
  coin_active = true;
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
        digitalWrite(itemSelect.pin, HIGH);
        Serial.println(itemSelect.pin);
      }
    } else printError(result);
  }
}

void printError(EDB_Status err) {
    Serial.print("ERROR: ");
    switch (err) {
        case EDB_OUT_OF_RANGE:
            Serial.println("Plates out of range");
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
            Serial.print("Plate: ");
            Serial.print(plates);
            Serial.print(" ID: ");
            Serial.print(itemSelect.id);
            Serial.print(" Active: ");
            Serial.println(itemSelect.active);
        }
        else printError(result);
    }
}
