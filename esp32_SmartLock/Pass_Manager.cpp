
#include "Pass_Manager.h"
#include <EEPROM.h>

// Define external variables
char password[6] = "00000";
unsigned char index_t = 0;
unsigned int error_num = 0;
char pass_def[6] = "12345";
char change_pass[6]="*****";
char camera[6]="AAAAA"; // quet cammera
bool accept_change=false;
bool mode_auto =true;
bool request_close=false;
// Internal buffers
char data_input[6];
char new_pass1[6];
char new_pass2[6];
unsigned char in_num = 0;
int time_hold = 10;  // time in Second to hold door before close door
bool update_pass1= false;
bool keyboard =true;
//*****************************************************************************************************
//Ghi dữ liệu vào EEPROM
void writeEEPROM(char data[]) {
  for (unsigned char i = 0; i < 5; i++) {
    EEPROM.write(i, data[i]);
  }
  EEPROM.commit();
}
// Đọc dữ liệu từ EEPROM và gắn vào password
void readEEPROM() {
  for (unsigned char i = 0; i < 5; i++) {
    password[i] = EEPROM.read(i);
  }
}

void insertData(char data1[], const char data2[])  // Assign buffer 2 to buffer 1
{
  for (unsigned char i = 0; i < 5; i++) {
    data1[i] = data2[i];
  }
}

void clear_data_input()  // Clear current input data
{
  for (int i = 0; i < 6; i++) {
    data_input[i] = '\0';
  }
}

unsigned char isBufferdata(const char data[])  // Check if buffer has 5 characters
{
  for (unsigned char i = 0; i < 5; i++) {
    if (data[i] == '\0') {
      return 0;
    }
  }
  return 1;
}

bool compareData(const char data1[], const char data2[]) {
  for (int i = 0; i < 5; i++) {
    if (data1[i] != data2[i]) {
      return false;
    }
  }
  return true;
}

void getData()  // Receive buffer from keypad
{
  char key = keypad.getKey();  // Read key value
  if (key) {
    Serial.println(key);
    if (in_num < 5) {
      data_input[in_num] = key;
      lcd.setCursor(5 + in_num, 1);
      lcd.print(data_input[in_num]);
      delay(200);
      lcd.setCursor(5 + in_num, 1);
      lcd.print("*");
      in_num++;
    }

    if (in_num == 5) {
      Serial.println(data_input);
      in_num = 0;
      delay(500);
      lcd.clear();
    }
  }
}

void checkPass() {
  getData();
  if (isBufferdata(data_input)) {
    lcd.clear();
    lcd.setCursor(5, 0);
    if (compareData(data_input, password)) {

      index_t = 1;
    } 
    else if (compareData(data_input, camera)) {
      index_t = 7;
    } 
    else if (compareData(data_input, change_pass)&&(accept_change==true))
    {
      Serial.println(" CHANGE PASS OFFLINE:");
      index_t=2;
    }
    else {

      error_num++;

      index_t = 4;
    }
    clear_data_input();
  }
}

void change_Pass()  // Change password
{
  lcd.setCursor(0, 0);
  lcd.print("-- Change Pass --");
  delay(2000);
  lcd.setCursor(0, 0);
  lcd.print("--- New Pass ---");
  while (1) {
    getData();
    if (isBufferdata(data_input)) {
      insertData(new_pass1, data_input);
      clear_data_input();
      break;
    }
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("---- AGAIN ----");
  while (1) {
    getData();
    if (isBufferdata(data_input)) {
      insertData(new_pass2, data_input);
      clear_data_input();
      break;
    }
  }
  delay(700);
  if (compareData(new_pass1, new_pass2)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("--- Success ---");
    delay(1000);
    writeEEPROM(new_pass2);
    insertData(password, new_pass2);
    lcd.clear();
    update_pass1=true;
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("-- Mismatched --");
    delay(1000);
    lcd.clear();
  }
   // accept_change=false;
    index_t = 0;
}

void reset_Pass() {
  writeEEPROM(pass_def);  // Save default password to EEPROM
  readEEPROM();           // Read password from EEPROM
  Serial.println("Password : ");
  Serial.println(password);
  lcd.setCursor(0, 0);
  lcd.print("--Reset Success--");
  delay(1000);
  lcd.clear();
  index_t = 0;
  update_pass1=true;
}

void OpenDoor_modeauto(int time_hold) {
  // Open the door
  Serial.println("OPENING DOOR");
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("OPENING");
  
  digitalWrite(relayPin, HIGH); // Turn on relay
  // for (int posDegrees = 0; posDegrees <= 90; posDegrees++) {
  //   servo1.write(posDegrees);
  //   delay(20);
  // }
  delay(30);
  servo1.write(0);
  lcd.clear();
  // Countdown for the door to remain open
  for (unsigned int i = 0; i < time_hold; i++) {
    unsigned int time_remaining = time_hold - i;
    lcd.setCursor(0, 0);
    lcd.print("Close in:");
    lcd.setCursor(6, 1);
    lcd.print(time_remaining); // Display time remaining
    lcd.print("second ");
    
    Serial.print("DOOR CLOSE in: ");
    Serial.print(time_remaining);
    Serial.println(" seconds");
    
    delay(1000); // Wait 1 second
  }
    CloseDoor();
  // Close the door
  }
  void OpenDoor_modenoauto(){
     Serial.println("OPENING DOOR");
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("OPENING");
  
  digitalWrite(relayPin, HIGH); // Turn on relay
    delay(30);
  servo1.write(0);
  // for (int posDegrees = 0; posDegrees <= 90; posDegrees++) {
  //   servo1.write(posDegrees);
  //   delay(20);
  //}
  lcd.clear();
    lcd.setCursor(4, 0);
  lcd.print("PUSH BUTTON");
  }
void CloseDoor(){
  Serial.println("DOOR CLOSING");
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("CLOSING!");
delay(30);
servo1.write(90);
  // for (int posDegrees = 90; posDegrees >= 0; posDegrees--) {
  //   servo1.write(posDegrees);
  //   delay(20);
  // }

  delay(1000); // Wait before clearing the LCD
  lcd.clear();

  digitalWrite(relayPin, LOW); // Turn off relay
  Serial.println("DOOR CLOSED");
  delay(20);
  request_close =false;
  error_num=0; 
  index_t=0;
}

void Warring() {
  lcd.clear();
  for (int i = 1; i < 10; i++) {
    lcd.setCursor(3, 0);
    lcd.print(" Warring!");
    delay(500);
    lcd.clear();
    delay(500);
  }
  error_num = 0;
  if(!keyboard)
  { index_t=6;}
  else{
  index_t =0;
}
}

void fail_Pass() {
  if (error_num >= 5) {
    index_t=5;
  } else {
     lcd.setCursor(2, 0);
    lcd.print("Wrong ");
    lcd.print(error_num);
    lcd.print(" Times");
    delay(500);
    lcd.clear();
      index_t = 0;
  }
}
