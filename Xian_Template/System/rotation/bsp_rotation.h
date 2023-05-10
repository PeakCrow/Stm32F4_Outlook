/*******************************************************************************
  * @file        bsp_rotation_sensor.h
  * @version     v1.0.0
  * @copyright   COPYRIGHT &copy; 2022 CSG
  * @author      TRX
  * @date        2022-6-15
  * @brief
  * @attention
  * Modification History
  * DATE         DESCRIPTION：轮速传感器初始化
  * ------------------------
  * - 2022-6-15  TRX Created
*******************************************************************************/

#ifndef _BSP_ROTATION_SENSOR_H_
#define _BSP_ROTATION_SENSOR_H_

#include "sys.h"


void bsp_InitRotationSensor(void);
float Rotation_Sensor_Get(uint8_t _gear_num,float _wheel_radius);



#endif

