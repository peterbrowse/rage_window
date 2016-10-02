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

#define TABLE_SIZE 512
#define RECORDS_TO_CREATE 9

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
  "Press '*' to confirm.",
  "Cancelled...",
  "Have a great day.",
  "Sold out.",
  "Not available.",
  "Press '#' to cancel."
};

String selection;
 
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  Serial.print("Activating Rage Vending Machine...");
  lcd.print("Starting Rage...");
  Serial.println();

  db.create(0, TABLE_SIZE, sizeof(itemSelect));

  //A1
  sprintf(itemSelect.id, "A1", i++);
  itemSelect.pin = 7;
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //A2
  sprintf(itemSelect.id, "A2", i++);
  itemSelect.pin = 7;
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //A3
  sprintf(itemSelect.id, "A3", i++); 
  itemSelect.pin = 7;
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //B1
  sprintf(itemSelect.id, "B1", i++); 
  itemSelect.pin = 7;
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //B2
  sprintf(itemSelect.id, "B2", i++); 
  itemSelect.pin = 7;
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //B3
  sprintf(itemSelect.id, "B3", i++); 
  itemSelect.pin = 7;
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //C1
  sprintf(itemSelect.id, "C1", i++); 
  itemSelect.pin = 7;
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //C2
  sprintf(itemSelect.id, "C2", i++); 
  itemSelect.pin = 7;
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  //C3
  sprintf(itemSelect.id, "C3", i++);  
  itemSelect.pin = 7;
  itemSelect.active = true;
  db.appendRec(EDB_REC itemSelect);

  delay(2000);
  Serial.print("Item Count: ");
  Serial.println(db.count());
  lcd.clear();
  lcd.print("Plates: " + String(db.count()));
  delay(2000);
  lcd.clear();
  lcd.print(messages[0]);
}

void loop() {

  
  char key = customKeypad.getKey();

  if(key) {
    switch (key)
    {
      case '*':
        boolean result;
        boolean activated;
        result = check_result();
        if (result) {
          activated = smash_plate();
          if(activated) {
            selection = {""};
            Serial.println(messages[3]);
            lcd.clear();
            lcd.print(messages[3]);
          } else {
            selection = {""};
            Serial.println(messages[4]);
            lcd.clear();
            lcd.print(messages[4]);
          }
        } else {
          selection = {""};
          Serial.println(messages[5]);
          lcd.clear();
          lcd.print(messages[5]);
        }
        break;
      case '#':
        selection = {""};
        Serial.println(messages[2]);
        lcd.clear();
        lcd.print(messages[2]);
        break;
      default:
        String theKey = String(key);
        selection = selection + theKey;
        Serial.println(selection);
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(selection);
    } 
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
