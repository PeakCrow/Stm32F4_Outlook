#ifndef ABOUT_PHONE_H
#define ABOUT_PHONE_H
#include "lvgl.h"
#include "monitor.h"

#define screen_left_pos 30
#define str_about_phone_title "关于本机"
#define str_lvgl_version "current_lvgl_version is #ff0000 %d.%d.%d#"
#define str_fatfs_version "current_fatfs_revision_id is #ff0000 %d#"
#define str_rtos_version "azure_rtos_threadx_version is #ff0000 %d.%d.%d#"
#define str_author_version "software_and_hardware_author is #ff0000 runxian.tian#"
#define str_ls_racing_fleet "ls_racing_fleet is #00ff00 E01#"
#define str_board_hardware_version "onboard_hardware #ff0000 STM32F407|LCD|FLASH|CAN #\n"\
                              "#ff0000 MLX90614|ADS1256|WS2812B|EEPROM|SDIO#"
#define str_cmsis_version "stm32f4xx_cmsis_version is #ff0000 %d.%d.%d.%d#"
#define str_hardware_version "hardware_version is #ff0000 %d.%d.%d#"

void About_Phone_Ui(lv_obj_t *parent);

#endif // MOTOR_CONTROL_H
