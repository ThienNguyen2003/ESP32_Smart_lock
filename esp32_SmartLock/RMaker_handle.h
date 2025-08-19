#ifndef RMAKER_HANDLE_H
#define RMAKER_HANDLE_H

#include "RMaker.h"
#include "WiFi.h"
#include "WiFiProv.h"
#include "Pass_Manager.h"

// Khai báo các biến và hàm
extern bool lock_state;
extern const char *service_name;
extern const char *pop;
extern char pass_buffer[6];
extern int num[5];

// Khai báo thiết bị
extern Device lock;
extern Device changepass;
extern Device getweb;
extern Device warning;
// Hàm xử lý sự kiện Provisioning
void sysProvEvent(arduino_event_t *sys_event);
// Callback xử lý mở cửa 
void Callback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx);
// Callback xử lý thay đổi mật khẩu
// void passCallback(Device *device, Param *param, const param_val_t val, void *priv_data, write_ctx_t *ctx);

// Hàm cấu hình và khởi tạo ứng dụng RainMaker
void config_app_RMaker();

#endif // RMAKER_HANDLE_H
