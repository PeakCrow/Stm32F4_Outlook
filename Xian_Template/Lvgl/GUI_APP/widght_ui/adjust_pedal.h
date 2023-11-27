#ifndef ADJUST_PEDAL_H
#define ADJUST_PEDAL_H
#include "lvgl.h"
#include "monitor.h"

/*存储在EEPROM中的车手位置数据大小，一个位置数据占据两个字节*/
#define Drive_Pos_Size 2
#define str_adjust_pedal_title " 可调制动踏板 "
#define str_pedal_pos "Pedal_Pos:%.2f"
#define str_forward_btn "FORWARD"
#define str_reverse_btn "REVERSE"
#define str_shild_switch "Adjust On"
#define str_driver_switch "Driver %d"
#define str_record_btn "RECORD"
#define str_zero_btn "ZERO"
#define str_driver_pos "Driver %d Pos %d.%d "
#define str_pedal_pos_ "Pedal_Pos : %.2f"
#define str_adjust_on "Adjust On"
#define str_adjust_off "Adjust Off"
#define str_deriver_pos_label "Driver %d Pos %.2f "

typedef struct _driverx_pos {
	float driver1_pos;
	float driver2_pos;
	float driver3_pos;
	float current_pos;
}st_driver_pos;

extern st_driver_pos DriverX_Pos;

void Adjust_Pedal_Ui(lv_obj_t *parent);

#endif // MOTOR_CONTROL_H
