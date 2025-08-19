#include "RMaker_handle.h"

// Định nghĩa các biến toàn cục
bool lock_state = false;
int num[5] = { 0, 0, 0, 0, 0 };
char pass_buffer[6]="00000";
const char *service_name = "PROV_smart_lock";
const char *pop = "abcd1234";



// Định nghĩa thiết bị
Device lock("LOCK", "esp.device.lock", NULL);
Device changepass("CHANGE PASS", "esp.device.security-panel", NULL);
Device getweb("GET WEB CAM","esp.device.tv",NULL);
Device warning("WARNING","esp.device.doorbell",NULL);
// Hàm xử lý sự kiện Provisioning
void sysProvEvent(arduino_event_t *sys_event) {
  switch (sys_event->event_id) {
    case ARDUINO_EVENT_PROV_START:
      Serial.printf("\nProvisioning Started with name \"%s\" and PoP \"%s\" on BLE\n", service_name, pop);
      WiFiProv.printQR(service_name, pop, "ble");
      break;
    case ARDUINO_EVENT_PROV_INIT:
      WiFiProv.disableAutoStop(10000);
      break;
    case ARDUINO_EVENT_PROV_CRED_SUCCESS:
      WiFiProv.endProvision();
      break;
    default:
      break;
  }
}


void Callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx) {
  const char *device_name = device->getDeviceName();
  const char *param_name = param->getParamName();
  ///////////////////////////////device lock////////////////////////
  if (strcmp(device_name, "LOCK") == 0) {
    if (strcmp(param_name, "UNLOCK") == 0) {

      lock_state = val.val.b;
      if(lock_state){
      Serial.printf("Received value =MỞ for Lock \n");
      index_t =1;
    }
    else if (!lock_state){
        request_close=true;
      Serial.printf("Nhận lệnh đóng cửa \n");
    }
      param->updateAndReport(val);
    }
    else if(strcmp(param_name, "MODE OPEN DOOR") == 0){
       const char *mode = val.val.s;
    if (strcmp(mode, "AUTO CLOSE") == 0) {
      mode_auto=true;
      Serial.println(" AUTO CLOSE DOOR : ON");
            esp_rmaker_raise_alert("ĐÃ BẬT CHẾ ĐỘ TỰ ĐỘNG ĐÓNG CỬA");
    }
     else if (strcmp(mode, "JUST OPEN") == 0) {
      mode_auto=false;
            esp_rmaker_raise_alert("ĐÃ TẮT CHẾ ĐỘ TỰ ĐỘNG ĐÓNG CỬA");
      Serial.println(" AUTO CLOSE DOOR : OFF");
    }
        param->updateAndReport(val);
    }
    else if (strcmp(param_name, "TIME HOLD DOOR (in second)") == 0) {
      time_hold=val.val.i;
      Serial.print(" Time hold has just changed: ");
      Serial.println(time_hold);
      param->updateAndReport(val);
    }
   }

   /////////////////////////device change pass////////////////////////////
   else  if (strcmp(device_name, "CHANGE PASS") == 0) {
    if(strcmp(param_name, "Change pass offline:")==0){
      bool value=val.val.b;
      if(value){
        Serial.println(" CHO PHEP CHANGE PASS OFFLINE");
        accept_change=true;
      }
      else if (!value){
        Serial.println(" KHONG CHO PHEP CHANGE PASS OFFLINE");
        accept_change =false;
      }
     param->updateAndReport(val);
    }
    else if (strncmp(param_name, "Number", 6) == 0) {
      int index = param_name[6] - '1';
      if (index >= 0 && index < 5 && val.val.i != NULL) {
        num[index] = val.val.i;
        Serial.printf("Updated num[%d] = %i\n", index, num[index]);
        param->updateAndReport(val);
      }
    } else if (strcmp(param_name, "Save Pass") == 0) {
      Serial.println("Password saved successfully!");
      for (int i = 0; i < 5; i++) {
        pass_buffer[i] = num[i] + '0';
      }
      pass_buffer[5] = '\0';
     writeEEPROM(pass_buffer);
      readEEPROM();
      esp_rmaker_raise_alert("MẬT KHẨU CỦA KHÓA VỪA ĐƯỢC THAY ĐỔI");
      Serial.println("======================");
      Serial.println(password);
      Serial.println("======================");
      param->updateAndReport(val);
      device->updateAndReportParam("PASS:", password);
    }
  }
  /////////////////////////////////////////////////////////////////////////////////////////////
  else   if (strcmp(device_name,"WARNING") == 0) {
    if (strcmp(param_name,"WARNING_button") == 0) {
     bool warning_state = val.val.b;
      if(warning_state){
      Serial.printf("Received value = %s for WARNING \n", val.val.b ? "BAO DONG " : "KHONG BAO DONG");
      index_t =5;
    }
     param->updateAndReport(val);
    }
    else if (strcmp(param_name, "KEYBOARD") == 0) {
    const char *mode = val.val.s;
    if (strcmp(mode, "ENABLE") == 0) {
      lcd.clear();
      index_t=0;
      keyboard= true;
      Serial.println(" Password : ON");
            esp_rmaker_raise_alert("MẬT KHẨU CỦA KHÓA ĐÃ ĐƯỢC BẬT");
    }
     else if (strcmp(mode, "DISABLE") == 0) {
            esp_rmaker_raise_alert("MẬT KHẨU CỦA KHÓA ĐÃ BỊ VÔ HIỆU HÓA");
             lcd.clear();
      index_t =6;
      keyboard =false;
      Serial.println(" Password : OFF");
    }
        param->updateAndReport(val);
    }
}
}
// Hàm cấu hình và khởi tạo ứng dụng RainMaker
void config_app_RMaker() {
  strcpy(pass_buffer, password);
  Node my_node = RMaker.initNode("SMART LOCK");
/////////////////////////////////////////////////////////////////////////////
  Param lock_button("UNLOCK",NULL, value(false),PROP_FLAG_READ | PROP_FLAG_WRITE);
  lock_button.addUIType(ESP_RMAKER_UI_PUSHBUTTON);
  lock.addParam(lock_button);
  static const char *doormodes[] = {"AUTO CLOSE", "JUST OPEN"};
  Param door_mode("MODE OPEN DOOR", ESP_RMAKER_PARAM_MODE, value("AUTO CLOSE"), PROP_FLAG_READ | PROP_FLAG_WRITE);
  door_mode.addValidStrList(doormodes, 2);
  door_mode.addUIType(ESP_RMAKER_UI_DROPDOWN);
  lock.addParam(door_mode);
  Param time_hold_door("TIME HOLD DOOR (in second)","custom.param.level", value(time_hold), PROP_FLAG_READ | PROP_FLAG_WRITE);
    time_hold_door.addBounds(value(5), value(60), value(5));
    time_hold_door.addUIType(ESP_RMAKER_UI_SLIDER);
    lock.addParam(time_hold_door);
  lock.assignPrimaryParam(lock.getParamByName("UNLOCK"));
  lock.addCb(Callback);

/////////////////////////////////////////////////////////////////////////////

  changepass.assignPrimaryParam(changepass.getParamByName(ESP_RMAKER_DEF_NAME_PARAM));
  Param change_offline("Change pass offline:",ESP_RMAKER_PARAM_TOGGLE, value(false), PROP_FLAG_READ | PROP_FLAG_WRITE);
  change_offline.addUIType(ESP_RMAKER_UI_TOGGLE);
  changepass.addParam(change_offline);
  Param current_pass("PASS:", "custom.param.string", value(password), PROP_FLAG_READ);
  changepass.addParam(current_pass);
  for (int i = 1; i <= 5; i++) {
    String param_name = "Number" + String(i);
    Param number(param_name.c_str(), "custom.param.level", value(0), PROP_FLAG_READ | PROP_FLAG_WRITE);
    number.addBounds(value(0), value(9), value(1));
    number.addUIType(ESP_RMAKER_UI_SLIDER);
    changepass.addParam(number);
  }
  Param save_button("Save Pass", NULL, value(true), PROP_FLAG_WRITE);
  save_button.addUIType(ESP_RMAKER_UI_TRIGGER);
  changepass.addParam(save_button);
  changepass.assignPrimaryParam(changepass.getParamByName("Save_Pass"));
  changepass.addCb(Callback);

///////////////////////////////////////////////////////////////////////////////////
   Param web("link web here:","custom.param.string",value("Cammera is no internet"),PROP_FLAG_READ | PROP_FLAG_WRITE);
   getweb.addParam(web);
/////////////////////////////////////////////////////////////////////////////////////////////////////
   Param warning_button("WARNING_button",NULL, value(false),PROP_FLAG_READ | PROP_FLAG_WRITE);
  warning_button.addUIType(ESP_RMAKER_UI_PUSHBUTTON);
  warning.addParam(warning_button);

  static const char *modes[] = {"ENABLE", "DISABLE"};
  Param using_password("KEYBOARD", ESP_RMAKER_PARAM_MODE, value("ENABLE"), PROP_FLAG_READ | PROP_FLAG_WRITE);
  using_password.addValidStrList(modes, 2);
  using_password.addUIType(ESP_RMAKER_UI_DROPDOWN);
  warning.addParam(using_password);
  warning.addCb(Callback);

   my_node.addDevice(lock); 
    my_node.addDevice(changepass);
  my_node.addDevice(warning);
  my_node.addDevice(getweb);



  RMaker.enableTZService();
  RMaker.enableSchedule();
  RMaker.start();
  WiFi.onEvent(sysProvEvent);
  WiFiProv.beginProvision(NETWORK_PROV_SCHEME_BLE, NETWORK_PROV_SCHEME_HANDLER_FREE_BTDM, NETWORK_PROV_SECURITY_1, pop, service_name);
}
