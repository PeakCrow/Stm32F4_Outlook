#ifndef ADJUST_PEDAL_H
#define ADJUST_PEDAL_H
#include "lvgl.h"
#include "monitor.h"

typedef struct _driverx_pos {
	float driver1_pos;
	float driver2_pos;
	float driver3_pos;
	float current_pos;
}st_driver_pos;

extern st_driver_pos DriverX_Pos;

void Adjust_Pedal_Ui(lv_obj_t *parent);

#endif // MOTOR_CONTROL_H
