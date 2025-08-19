#include <Arduino.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <ESP32Servo.h>
#include <EEPROM.h>
#include <HardwareSerial.h>
#include "Pass_Manager.h"
#include "RMaker_handle.h"
HardwareSerial SerialPort(2);
// Initialize LCD
char IPcam[16];
LiquidCrystal_I2C lcd(0x27, 16, 2);
int cursorColumn = 0; // Vị trí cột của LCD
int cursorRow = 0;    // Vị trí dòng của LCD

// Keypad configuration
const byte ROWS = 4; // Số hàng của keypad
const byte COLS = 4; // Số cột của keypad
// Định nghĩa các ký tự trên phím
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {13, 12, 14, 27}; /* connect to the row pinouts of the keypad */
byte colPins[COLS] = {26, 25, 33, 32}; /* connect to the column pinouts of the keypad */
// Tạo đối tượng Keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Relay and Servo configuration
const int relayPin = 2;  // Chân D12 kết nối với chân IN của relay
const int BuzzerPin=17;
int button_OPEN = 16; //nút mở cửa trong nhà
Servo servo1; // Tạo đối tượng servo

int servoPin = 15;  // Chân điều khiển servo
// ký tự đặt biệt hình ổ khóa trên LCD
byte lock_char[8]=
{B00000,
B01110,
B10001,
B10001,
B11111,
B11011,
B11111,
B11111};

static int gpio_0 = 0;
static int gpio_2 = 2;
static int gpio_23 = 23;
void setup() {
    // Initialize EEPROM
    EEPROM.begin(512);
    servo1.write(90);
    // Initialize Serial
    Serial.begin(115200); // Khởi tạo Serial để in kết quả
  SerialPort.begin(9600, SERIAL_8N1, 3, 1);
    // Initialize LCD
    lcd.init();
    lcd.backlight();  // Bật đèn nền của LCD
    lcd.setCursor(0, 1);  // Đặt con trỏ ở dòng 2, cột 0
    lcd.print("   SYSTEM INIT   ");
    delay(2000);
    lcd.clear();
      pinMode(gpio_0, INPUT);
       pinMode(button_OPEN, INPUT_PULLDOWN);  //Cài đặt chân D11 ở trạng thái đọc dữ liệu
       pinMode(gpio_23, INPUT_PULLDOWN);  //Cài đặt chân D11 ở trạng thái đọc dữ liệu

      pinMode(gpio_2, OUTPUT);
      pinMode(BuzzerPin, OUTPUT);
      digitalWrite(BuzzerPin, LOW); 

    // khởi tạo Relay
    pinMode(relayPin, OUTPUT);  // Thiết lập chân relay là đầu ra
    digitalWrite(relayPin, LOW); // Ban đầu tắt relay
    
    // khởi tạo Servo
    servo1.attach(servoPin);
    
    // đọc pass từ  EEPROM
//    writeEEPROM(pass_def);
    readEEPROM();
    
    // Display current password (for debugging purposes)
    Serial.print("PASSWORD: ");
    Serial.println(password);
    config_app_RMaker();

    lcd.createChar(0, lock_char);    // Tạo ký tự ô đen ở vị trí 0
}

void loop() {
  if (SerialPort.available())
  {
    String receiveIP =SerialPort.readStringUntil('\n');
    receiveIP.trim();
    receiveIP.toCharArray(IPcam,16);
    Serial.println( " IP camera:" + String(IPcam));
    delay(500);
    getweb.updateAndReportParam("link web here:", IPcam);
  }
 if (digitalRead(gpio_0) == LOW) {  // Push button pressed

    // Key debounce handling
    delay(100);
    int startTime = millis();
    while (digitalRead(gpio_0) == LOW) {
      int ok =millis()-startTime;
      delay(50);
      if(ok >10000){
        digitalWrite(gpio_2,HIGH);
      }
      }
    
    int endTime = millis();
    digitalWrite(gpio_2,LOW);
    if ((endTime - startTime) > 10000) {
      // If key pressed for more than 10secs, reset all
      Serial.printf("Reset to factory.\n");
      RMakerFactoryReset(2);
    } else if ((endTime - startTime) > 3000) {
      Serial.printf("Reset Wi-Fi.\n");
      // If key pressed for more than 3secs, but less than 10, reset Wi-Fi
      RMakerWiFiReset(2);
    }
 }
    
//server.handleClient();
    lcd.setCursor(1, 0);
    lcd.print("Enter Password");
    // Kiểm tra mật khẩu
    checkPass();
    
    /* Thực hiện hành động dựa trên index_t; 
      index_t=1 mở cửa

      index_t=2 đổi mật khẩu offline

      index_t=3 reset pass

      index_t=4 cảnh báo khi sai mật khẩu nhiều lần
    */
    
    while (index_t == 1)
    {  
       lock.updateAndReportParam("UNLOCK", true); 
       beep();
      digitalWrite(relayPin, HIGH);
         if (mode_auto){
        OpenDoor_modeauto(time_hold);
         }
        if (!mode_auto){
          OpenDoor_modenoauto();
          beep();
          delay(100);
          beep();
        while(1){
          if(request_close){
            break;
          }
           else if (digitalRead(button_OPEN) == HIGH) {  // Push button pressed

    // Key debounce handling
              delay(50);
              int startTime = millis();
              while (digitalRead(button_OPEN) == HIGH) {
                  delay(50);
                }
              int endTime = millis();
              if((endTime-startTime)>50){
                request_close =true;
              }
                  }
                }
                CloseDoor();
              }
              beep();
              digitalWrite(relayPin, LOW);
            lock.updateAndReportParam("UNLOCK", false); 
             if (keyboard==false){
      index_t=6;
    }
    }
    
   while (index_t == 2)
    {
        change_Pass();
    }
    while(index_t == 3)
    {
        reset_Pass();
    }
   while (index_t == 4)
    {
        fail_Pass();
    }
   while(index_t==5){
          digitalWrite(BuzzerPin, HIGH); 
        warning.updateAndReportParam("WARNING_button", true); 
         esp_rmaker_raise_alert("CHẾ ĐỘ CẢNH BÁO VỪA ĐƯỢC BẬT");
         Warring();
         warning.updateAndReportParam("WARNING_button", false); 
        digitalWrite(BuzzerPin, LOW); 
    }
    while (index_t==6){
        lcd.setCursor(0, 0);
        lcd.print("keyboard disable");
        lcd.setCursor(7, 1);
         lcd.write(byte(0));
         delay(2000);
    }
while (index_t == 7) {
    lcd.setCursor(0, 0);
    lcd.print("camera scanning");
    
    unsigned int counter = 9; // Đếm ngược 10 giây
    unsigned long start = millis(); // Lấy thời gian bắt đầu

    while (true) {
        lcd.setCursor(8, 1);
        lcd.print(counter); // In giá trị đếm ngược
        bool gpio_23state = digitalRead(gpio_23);
        if (gpio_23state == HIGH) {
          beep();
          beep();
          beep();
            index_t = 1; // Thay đổi trạng thái và thoát vòng lặp
            break;
        }
        unsigned long now = millis();
        if (now - start >= 1000) {
            counter--; // Giảm đếm ngược mỗi giây
            start = now; // Cập nhật lại thời gian bắt đầu
        }
        // Kiểm tra nếu hết thời gian
        if (counter == 0) {
          beep();
            index_t = 0; // Đặt trạng thái index_t về 0
            break;
        }
        delay(20); // Giảm tải cho CPU
    }
    lcd.clear(); // Xóa màn hình
    delay(50);
}
    if((update_pass1==true) ){
      changepass.updateAndReportParam("PASS:", password);
      delay(50);
       esp_rmaker_raise_alert("MẬT KHẨU CỦA KHÓA VỪA ĐƯỢC THAY ĐỔI");
    // update_pass =false;
      update_pass1 =false;
    }
     if (digitalRead(button_OPEN) == HIGH) { // Nếu mà button bị nhấn
       index_t=1;
      beep();

     }   
}
void beep(){
  digitalWrite(BuzzerPin, HIGH);
  delay(100);
  digitalWrite(BuzzerPin, LOW);
  delay(50);
}