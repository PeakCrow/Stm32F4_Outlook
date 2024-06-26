#ifndef MONITOR_H
#define MONITOR_H
#include <stdlib.h>
#include "lvgl/lvgl.h"
#include "log.h"

#define png_load_path(png) "A:/Ls_Monitor_Lower/LVGL_Monitor/widght_ui/images/"#png

void Gui_Monitor_App(void);
typedef void (*App_btn_Back_Cb_Ptr)(lv_event_t* e);
/* 每个按钮打开的页面公用的初始化函数 */
lv_obj_t* App_Common_Init(const char *title,App_btn_Back_Cb_Ptr App_btn_Back_Cb);


#endif // MONITOR_H

