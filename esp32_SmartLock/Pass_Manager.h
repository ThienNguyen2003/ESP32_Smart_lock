
#ifndef PASS_MANAGER_H
#define PASS_MANAGER_H
#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
extern LiquidCrystal_I2C lcd;
extern Keypad keypad;
extern Servo servo1;
extern const int relayPin;
extern bool update_pass1;
// Function declarations
void writeEEPROM(char data[]);
void readEEPROM();
void insertData(char data1[], const char data2[]);
void clear_data_input();
unsigned char isBufferdata(const char data[]);
bool compareData(const char data1[], const char data2[]);
void getData();
void checkPass();
void change_Pass();
void reset_Pass();
void fail_Pass();
void OpenDoor_modeauto(int time_hold);
void OpenDoor_modenoauto();
void CloseDoor();
void Warring();

// External variables
extern char password[6];
extern char pass_def[6];
extern char change_pass[6];
extern char camera[6];
extern bool mode_auto;
extern bool request_close;
extern bool accept_change;
extern unsigned char index_t;
extern unsigned int error_num;
extern  int time_hold;
extern bool keyboard;
#endif
