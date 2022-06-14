#ifndef _BSP_ROTATION_SENSOR_H_
#define _BSP_ROTATION_SENSOR_H_

void bsp_InitRotationSensor(void);
float Rotation_Sensor_Get(uint8_t _gear_num,float _wheel_radius,float _collectiontime);



#endif

