# Stm32F407zet6Template

#### 介绍
对曾经在学校社团的项目进行重构，是一个类似于信号采集的项目，偏向车载，开发板使用正点原子STM32F407核心板，软件方面使用非正点原子以外的教程。

#### 目录介绍
##### Xian_Template目录

| 文件夹  | 内容                                                         |
| ------- | ------------------------------------------------------------ |
| Core    | M4内核的启动文件                                             |
| Doc     | 文档数据、教程资料                 |
| FileX   | 实现FileX文件系统挂载SD卡|
| HAL_lib | ST提供的底层文件，HAL库                                      |
| Obj     | keil5编译产生的目标文件                                          |
| SYSTEM  | 包含有除main.c外的外设驱动文件                               |
| ThreadX | ThreadX操作系统的源文件                                      |
| User    | main.c文件和编译信息的txt文件(txt文件配合SI编辑器使用) |
| xian.si4project    | si编辑器项目 |

#### 实现功能

| 时间       | 内容                                                         |
| ---------- | ------------------------------------------------------------ |
| 2022-04-18 | 实现3个USART串口的FIFO驱动                                   |
| 2022-04-20 | 实现板载按键FIFO驱动                                         |
| 2022-05-03 | 实现板载FLASH的读写                                          |
| 2022-05-06 | 实现板载EEPROM芯片的读写                                     |
| 2022-05-11 | 实现微雪2.9寸三色墨水屏的驱动移植                            |
| 2022-05-26 | 实现1路CAN总线报文的读写                                     |
| 2022-06-07 | 实现PWM驱动，特别对PA6做了DMA数据输出                        |
| 2022-06-09 | 实现WS2812B-RGB(NUM:12)驱动,渐变灯、流水灯、特定序号、点亮特定序号段灯效 |
| 2022-06-15 | 实现1路霍尔式轮速传感器信号采集                              |
| 2022-06-21 | 实现ADS1256模块进行模拟量采集(8路，0-5v)                     |
| 2022-06-26 | 实现LCD显示与触摸驱动                                        |
| 2022-07-02 | 实现MLX90614红外温度传感器数据采集(iic总线在读写eeprom时400k，改为60k) |
| 2022-07-05 | 实现FileX文件系统挂载SD卡(底层驱动全部完成)                  |

#### 使用说明
- 打开工程，进入main函数，可以看到任务调度函数，并且开启了6个任务例程和简单的队列间通信的例程，其他的例如flashdemo、eepromdemo、墨水屏驱动，板载外设的驱动需要开启宏定义后者自行调用功能函数。
- 创建一个0.5s的软件定时器，不断调用轮速信号采集函数，PB6引脚输出500hz方波，接在PA15轮速传感器引脚上串口可以看到数据每0.5s打印一次。
- 按下keyup按键串口打印MLX90614红外温度传感器的温度。
- 板载的两个LED灯每隔一秒点亮一次，并且运行有正点原子触摸画板例程。
- (此状态会随着驱动的更新不断更新)。

#### 占用PIN脚
![输入图片说明](Xian_Template/Doc/%E5%B7%B2%E9%85%8D%E7%BD%AEPIN.png)
####  **LVGL模拟器效果** 
![输入图片说明](Xian_Template/Doc/PC%E6%A8%A1%E6%8B%9F%E5%99%A8%E6%95%88%E6%9E%9C.png)