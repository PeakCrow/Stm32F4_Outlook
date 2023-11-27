#ifndef MONITOR_H
#define MONITOR_H

#include "sys.h"

#define png_load_path(png) "0:/PICTURE/"#png
#define str_password "123456"
#define str_msg_title "ERROR"
#define str_msg_body "THE PASSWORD IS WRONG!"
#define str_confirm "#"
#define str_user_name "USER"

typedef void (*App_btn_Back_Cb_Ptr)(lv_event_t* e);


void Gui_Monitor_App(void);
/* 每个按钮打开的页面公用的初始化函数 */
lv_obj_t* App_Common_Init(const char *title,App_btn_Back_Cb_Ptr App_btn_Back_Cb);

#endif // MONITOR_H
