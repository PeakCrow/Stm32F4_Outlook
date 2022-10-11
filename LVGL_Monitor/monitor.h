#ifndef MONITOR_H
#define MONITOR_H
#include <stdlib.h>
#include "lvgl/lvgl.h"

void Gui_Monitor_App(void);
/* 每个按钮打开的页面公用的初始化函数 */
lv_obj_t* App_Common_Init(const char *title);

#endif // MONITOR_H
