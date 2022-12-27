#ifndef MONITOR_H
#define MONITOR_H

#include "lvgl.h"
#include "sys.h"

typedef void (*App_btn_Back_Cb_Ptr)(lv_event_t* e);

void Gui_Monitor_App(void);
/* 每个按钮打开的页面公用的初始化函数 */
lv_obj_t* App_Common_Init(const char *title,App_btn_Back_Cb_Ptr App_btn_Back_Cb);

#endif // MONITOR_H
