#ifndef BATTERY_BOX_H
#define BATTERY_BOX_H
#include <stdlib.h>
#include "lvgl/lvgl.h"
#include "widght_ui/monitor.h"
typedef enum _png_size{
    horzi_direct_png = 0,
    verti_direct_png = 1
} png_direction;

void Battery_Box_Ui(lv_obj_t *parent);

#endif // MOTOR_CONTROL_H
