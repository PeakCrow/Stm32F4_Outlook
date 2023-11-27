# 发现问题，记录问题，解决问题

------

<u>***除了必要的数据资料外，主要记录已经解决了的问题。***</u>

------

## 1.在进行页面切换时，任然会中断其他线程，使用两个led灯200ms闪烁，现象较为明显？

- [x] 使用了外部sram加速后，时间极大的缩短了，但是界面切换还是会打断整个系统，将GUI线程优先级放置为最低，那么不会打断系统，只会打断GUI线程本身

## 2.在led灯提高频率到200ms时，整个系统的死机概率上升太多？

- [ ] 

## 3.看门狗使用裸机系统调用没有问题，在RTOS的线程中调用就会失效？

- [ ] 


## 4.电池界面中的电池滚动效果在模拟器中可以实现，但在实际板子上却不可以？焯，有大问题！

- [ ] 可调踏板注意问题，电机控制板的电源不能停，因为每次上电的时候就是电机的0位置，目前解决方法就是拿一块电池进行持续的3V3供电，确保0位置是在正确的位置。*

## 5.切换回主界面时，有明显的加载时间，并且中断其他线程？

- [x] 目前通过使用屏幕显示内存全缓存的方式有极大缓解，目前性能消耗大小


    - IRAM:64KB/128KB CCM:2KB/64B ERAM:750KB/1024KB IFLASH:448K/1024K


## 6.keil5无法debug，并且报错cannot access target. shutting down debug session？

- [ ] 勾选Debug->Setting->Debug->Download Options->Verify Code Download/Download to Flash可以解决。

## 7.LVGL的定时器属性会打断RTOS的进程，就跟加载图片一样，但是动画属性也会，但是不如定时器属性明显，在传感器界面通过温度传感器和模拟量传感器数据的对比可以看出来？

- [ ] 


## 8.电池箱数据界面只能加载第一张图片，剩下的都不能加载，但是动画是可以正常运行的？

- [x] 原因是调用了API lv_obj_set_style_blend_mode(ui_Image_Particle2, LV_BLEND_MODE_ADDITIVE, LV_PART_MAIN | LV_STATE_DEFAULT);


## 9.加载的所有的图片都不能进行透明度的配置？

- [x] 不是透明度不能配置，而是此API不生效lv_obj_set_style_blend_mode，解决方法是用PS将图片的背景直接去掉。

## 10.cmsis-dap  stlink 无法在调试时打断点和在暂停后跳到程序运行位置，jlink则完全没问题，而且好像是在启动文件的188行->IMPORT  __main开始出错？

- [ ] 


## 11.进入HardFault_Handler()硬件错误中断的原因，根本原因目前还是没找到？

- [x] 在添加并使用segger的硬件错误中断检测文件之后，发现当前程序进入硬件错误中断的原因是在使用LVGL中的普通样式时，没有将lv_style_t style_btn;设置为静态局部变量，导致会跑死，另外在IAR的调试界面中的Call Stack栏中也可以看到是由于此函数中调用样式API造成的硬件错误中断。

## 12.在可调踏板界面中，再接收到数据后，是通过延时的方式，进行对应数据的发送，这种方法极其蠢蛋！？

- [ ] 可以考虑在bsp_usart_fifo.c文件中的368行的接收回调函数添加一个串口空闲中断的条件与来进行优化，可以使用全局变量标志，或者使用threadx中的事件标志组进行处理
- [ ] 要使用串口的空闲中断进行发送

## 13.Keil进行debug的时候，代码跑到启动文件的193行,LDR R0, = SystemInit 这行代码会报错误，Cannot access target Shutting down debug session. 另外只下载程序也同样跑不起来？

- [x] 同样的代码在IAR的环境却可以正常跑，而且不会进入硬件fault。使用问题6的方法可以在debug模式里跑，退出了反而不能跑，而且Command窗口会报错Memory verification error，使用IAR不下载程序只debug发现报了硬件故障，内容如下：处理器已将可配置优先级异常升级为HardFault，指令预取(CFSR.IBUSERR BFAR)上发现总线故障，故障位置0x8005abe，keil的问题是优化等级的问题，只能使用default等级，使用其他等级都会跑死到tx_thread_stack_analyze.c中，并且使用安富莱的优化过的SEGGER_HardFaultHandler.c文件中会打印 Enter HardFault_Handler, System Halt ，优化等级为0会造成threadx的性能站分析.c文件硬件故障，另外UI文件不能放在CCM RAM里，会无法运行TFT LCD任务

![](/Doc/images/同样的程序iar可以跑,keil不行,死在LCD任务上.png)

## 14.keil在debug状态下进行界面的切换不会打断整个系统的运行，也就是说看不到led灯停止闪烁的现象（与问题13相关联）！？

- [x] 这个问题不是由于debug模式的原因，只要把LCD任务的优先级放到最低就可以不打断其他任务的运行，至少现象上是看不出来的，但是UI自身的卡顿就会暴露出来。

## 15.sd卡中的图片的bin文件读取也会失败，需要重启才能正常加载出来？

- [ ] 

## 16.Wrong parameters value: file J:\Ls_Monitor_Lower\Xian_Template\System\spi\bsp_spi_bus.c on line 273 DMA搬运spi的外部flash字库会出现报错？使用DMA与INT模式都有问题，目前依托OS使用POLLING模式运行！！

- [ ] 当前通过debug发现在dma tx时，__HAL_LOCK()中的hdmatx总是locked状态，然后直接退出函数，导致HAL_SPI_TransmitReceive_IT()接口报错，

## 17.使用MDK编译的代码会使屏幕有条纹状的闪烁，但是IAR编译的代码却不会这样？

- [ ] 

## 18.关于每个界面的标题汉字，直接使用没有问题，但是用宏定义代替后，则无法显示？

- [ ] 

## 19.使用SecureRCT软件，会持续收到 k0按键按下 的打印，使用其他软件则接收不到，使用debug打印确实也有进行触发，使用其他软件在debug模式下缺不会进行触发？

- [ ] 

## 20.DispTaskInfo()接口目前无法正常跑出各个任务的数据来？

- [ ] 
