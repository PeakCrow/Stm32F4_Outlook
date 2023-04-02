# 发现问题，记录问题，解决问题

------

<u>***除了必要的数据资料外，主要记录已经解决了的问题。***</u>

------

## 在进行页面切换时，任然会中断其他线程，使用两个led灯200ms闪烁，现象较为明显？

- 


## 在led灯提高频率到200ms时，整个系统的死机概率上升太多？

- 


## 看门狗使用裸机系统调用没有问题，在RTOS的线程中调用就会失效？

- 


## 电池界面中的电池滚动效果在模拟器中可以实现，但在实际板子上却不可以？焯，有大问题！

- *可调踏板注意问题，电机控制板的电源不能停，因为每次上电的时候就是电机的0位置，目前解决方法就是拿一块电池进行持续的3V3供电，确保0位置是在正确的位置。*

## 切换回主界面时，有明显的加载时间，并且中断其他线程？

- 


## keil5无法debug，并且报错cannot access target. shutting down debug session？

- 勾选Debug->Setting->Debug->Download Options->Verify Code Download/Download to Flash可以解决。

## LVGL的定时器属性会打断RTOS的进程，就跟加载图片一样，但是动画属性也会，但是不如定时器属性明显，在传感器界面通过温度传感器和模拟量传感器数据的对比可以看出来？

- 


## 电池箱数据界面只能加载第一张图片，剩下的都不能加载，但是动画是可以正常运行的？

- 原因是调用了API lv_obj_set_style_blend_mode(ui_Image_Particle2, LV_BLEND_MODE_ADDITIVE, LV_PART_MAIN | LV_STATE_DEFAULT);


## 加载的所有的图片都不能进行透明度的配置？

- 不是透明度不能配置，而是此API不生效lv_obj_set_style_blend_mode，解决方法是用PS将图片的背景直接去掉。

## cmsis-dap  stlink 无法在调试时打断点和在暂停后跳到程序运行位置，jlink则完全没问题，而且好像是在启动文件的188行->IMPORT  __main开始出错？

- 


## 进入HardFault_Handler()硬件错误中断的原因，根本原因目前还是没找到？

- 在添加并使用segger的硬件错误中断检测文件之后，发现当前程序进入硬件错误中断的原因是在使用LVGL中的普通样式时，没有将lv_style_t style_btn;设置为静态局部变量，导致会跑死，另外在IAR的调试界面中的Call Stack栏中也可以看到是由于此函数中调用样式API造成的硬件错误中断。
