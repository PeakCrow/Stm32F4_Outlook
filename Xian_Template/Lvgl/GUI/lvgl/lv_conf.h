/**
 * @file lv_conf.h
 * Configuration file for v8.2.0
 */

/*
 * Copy this file as `lv_conf.h`
 * 1. simply next to the `lvgl` folder
 * 2. or any other places and
 *    - define `LV_CONF_INCLUDE_SIMPLE`
 *    - add the path as include path
 */
/*
* 以`lv_conf.h` 复制这个文件
 * 1. 跟lvgl文件夹放在一起
 * 2. 或者其他任何地方
 *    - 宏定义了 `LV_CONF_INCLUDE_SIMPLE`
 *    - 添加路径作为包含路径
 */
/* clang-format off */
#if 1 /*Set it to "1" to enable content*/

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS--颜色设置
 *====================*/

/*Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
/*颜色深度: 1 (每像素1字节), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#define LV_COLOR_DEPTH 16

/*Swap the 2 bytes of RGB565 color. Useful if the display has an 8-bit interface (e.g. SPI)*/
/*交换RGB565颜色的2字节. 显示器具有8位接口，例如SPI (e.g. SPI)*/
#define LV_COLOR_16_SWAP 0

/*Enable more complex drawing routines to manage screens transparency.
 *Can be used if the UI is above another layer, e.g. an OSD menu or video player.
 *Requires `LV_COLOR_DEPTH = 32` colors and the screen's `bg_opa` should be set to non LV_OPA_COVER value*/
 /*使用更复杂的绘图例程来管理屏幕透明度.
 *如果UI位于另一层之上，例如OSD菜单或者视频播放器，则可以使用
 *需要LV_COLOR_DEPTH=32颜色，屏幕的bg_opa应该设置为非LV_OPA_COVER值*/
#define LV_COLOR_SCREEN_TRANSP 0

/* Adjust color mix functions rounding. GPUs might calculate color mix (blending) differently.
 * 0: round down, 64: round up from x.75, 128: round up from half, 192: round up from x.25, 254: round up */
 /* 通过四舍五入调整颜色混合函数.GPU可能以不同的方式计算颜色混合.
 * 0：向下取证, 64: 从x.75向上取整, 128: 从一半向上取舍, 192: 从x.25向上取整数, 254: 向上取圆 */
#define LV_COLOR_MIX_ROUND_OFS (LV_COLOR_DEPTH == 32 ? 0: 128)

/*Images pixels with this color will not be drawn if they are chroma keyed)*/
/*图像如果使用色度键控，则不会绘制此颜色的像素*/
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)         /*pure green*/

/*=========================
   MEMORY SETTINGS--内存设置
 *=========================*/

/*1: use custom malloc/free, 0: use the built-in `lv_mem_alloc()` and `lv_mem_free()`*/
/*1: 使用自定义的malloc/free,0：使用内置的‘lv_mem_alloc() 和 lv_mem_free() */
#define LV_MEM_CUSTOM 0
#if LV_MEM_CUSTOM == 0
    /*Size of the memory available for `lv_mem_alloc()` in bytes (>= 2kB)*/
	/*可用于’lv_mem_alloc()'分配的内存大小，以字节为单位，大于等于2Kb*/
    #define LV_MEM_SIZE (80U * 1024U)          /*[bytes]*/

    /*Set an address for the memory pool instead of allocating it as a normal array. Can be in external SRAM too.*/
	/*设置内存池的地址，而不是将其分配为普通数组，也可以在外部sram中.*/
    #define LV_MEM_ADR  0    /*0: unused*/
    /*Instead of an address give a memory allocator that will be called to get a memory pool for LVGL. E.g. my_malloc*/
	/*代替地址给出一个内存分配器，他将被调用来获得LVGL的内存池，例如 my_malloc*/
    #if LV_MEM_ADR == 0
        //#define LV_MEM_POOL_INCLUDE "malloc.h"  /* Uncomment if using an external allocator--如果使用外部分配器，请取消注释*/
        //#define LV_MEM_POOL_ALLOC   lv_mymalloc          /* Uncomment if using an external allocator--如果使用外部分配器，请取消注释*/
    #endif

#else       /*LV_MEM_CUSTOM*/
    #define LV_MEM_CUSTOM_INCLUDE  <stdlib.h>	//<stdlib.h>    "malloc.h" /*Header for the dynamic memory function*/
    #define LV_MEM_CUSTOM_ALLOC   malloc	//malloc  lv_mymalloc
    #define LV_MEM_CUSTOM_FREE    free		//free  lv_myfree
    #define LV_MEM_CUSTOM_REALLOC realloc//realloc  lv_myrealloc
#endif     /*LV_MEM_CUSTOM*/

/*Number of the intermediate memory buffer used during rendering and other internal processing mechanisms.
 *You will see an error log message if there wasn't enough buffers. */
 /*渲染和其他处理机制期间使用的中间内存缓冲区的数量.
 *如果缓冲区不足，你将看到错误日期消息. */
#define LV_MEM_BUF_MAX_NUM 16

/*Use the standard `memcpy` and `memset` instead of LVGL's own functions. (Might or might not be faster).*/
/*使用标准的 `memcpy` and `memset` ，而不是LVGL自己的功能(可能更快，也可能不会更快).*/
#define LV_MEMCPY_MEMSET_STD 0

/*====================
   HAL SETTINGS--HAL设置
 *====================*/

/*Default display refresh period. LVG will redraw changed areas with this period time*/
/* 默认显示刷新周期，LVGL将在此期间重新绘制更改区域 */
#define LV_DISP_DEF_REFR_PERIOD 30      /*[ms]*/

/*Input device read period in milliseconds*/
/*输入设备读取周期*/
#define LV_INDEV_DEF_READ_PERIOD 30     /*[ms]*/

/*Use a custom tick source that tells the elapsed time in milliseconds.
 *It removes the need to manually update the tick with `lv_tick_inc()`)*/
 /*使用以毫秒为单位表示经过时间的自定义刻度源.
 *他消除了使用`lv_tick_inc()`手动更新信号的需要*/
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE "sys.h"         /*Header for the system time function*/
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (tx_time_get())    /*Expression evaluating to current system time in ms*/
#endif   /*LV_TICK_CUSTOM*/

/*Default Dot Per Inch. Used to initialize default sizes such as widgets sized, style paddings.
 *(Not so important, you can adjust it to modify default sizes and spaces)*/
 /*默认像素点的每英寸大小，用于初始化默认大小，如小部件大小，样式填充.
 *不太重要，你可以调整他以修改默认大小和空间*/
#define LV_DPI_DEF 130     /*[px/inch]*/

/*=======================
 * FEATURE CONFIGURATION--特征配置
 *=======================*/

/*-------------
 * Drawing--图画
 *-----------*/

/*Enable complex draw engine.
 *Required to draw shadow, gradient, rounded corners, circles, arc, skew lines, image transformations or any masks*/
 /*启动复杂绘图引擎
 *需要绘制阴影、渐变、圆角、圆、圆弧、斜线、图像变换或任何遮罩*/
#define LV_DRAW_COMPLEX 1
#if LV_DRAW_COMPLEX != 0

    /*Allow buffering some shadow calculation.
    *LV_SHADOW_CACHE_SIZE is the max. shadow size to buffer, where shadow size is `shadow_width + radius`
    *Caching has LV_SHADOW_CACHE_SIZE^2 RAM cost*/
	/*允许缓冲一些阴影计算.
    *LV_SHADOW_CACHE_SIZE 是缓冲区的最大阴影大小，其中阴影大小为‘阴影宽度+半径`
    *缓存具有LV_SHADOW_CACHE_SIZE^2 大小的RAM成本*/
    #define LV_SHADOW_CACHE_SIZE 0

    /* Set number of maximally cached circle data.
    * The circumference of 1/4 circle are saved for anti-aliasing
    * radius * 4 bytes are used per circle (the most often used radiuses are saved)
    * 0: to disable caching */
	/*设置最大缓存的圆圈数据数
    * 保存1/4的周长以进行抗锯齿
    * ”半径“每个圆圈使用4字节(保存最常用的半径)
    * 0：禁用缓冲*/
    #define LV_CIRCLE_CACHE_SIZE 4
#endif /*LV_DRAW_COMPLEX*/

/*Default image cache size. Image caching keeps the images opened.
 *If only the built-in image formats are used there is no real advantage of caching. (I.e. if no new image decoder is added)
 *With complex image decoders (e.g. PNG or JPG) caching can save the continuous open/decode of images.
 *However the opened images might consume additional RAM.
 *0: to disable caching*/
 /*默认图像缓存大小，图像缓存使图像保持打开状态
 *如果只使用内置图像格式，则缓存没有真正的优势，(即，如果没有添加新的图像解码器)
 *对于复杂的图像解码器(例如PNG或者JPG)，缓存可以保存图像的连续打开/解码
 *但是，打开的映像可能会消耗额外的RAM
 *0: 禁用缓存*/
#define LV_IMG_CACHE_DEF_SIZE   0

/*Number of stops allowed per gradient. Increase this to allow more stops.
 *This adds (sizeof(lv_color_t) + 1) bytes per additional stop*/
 /*每隔梯度允许的停止数，增加该值以允许更多的停止
 *每增加一个站点，就增加(sizeof(lv_color_t) + 1个字节*/
#define LV_GRADIENT_MAX_STOPS       2

/*Default gradient buffer size.
 *When LVGL calculates the gradient "maps" it can save them into a cache to avoid calculating them again.
 *LV_GRAD_CACHE_DEF_SIZE sets the size of this cache in bytes.
 *If the cache is too small the map will be allocated only while it's required for the drawing.
 *0 mean no caching.*/
 /*默认渐变缓冲区大小
 *当LVGL计算渐变贴图时，他可以将他们保存在缓存中，以避免再次计算他们
 *LV_GRAD_CACHE_DEF_SIZE 以字节为单位设置此缓存的大小.
 *如果缓存太小，则仅在图形需要时分配贴图
 *0 表示没有缓存.*/
#define LV_GRAD_CACHE_DEF_SIZE      0

/*Allow dithering the gradients (to achieve visual smooth color gradients on limited color depth display)
 *LV_DITHER_GRADIENT implies allocating one or two more lines of the object's rendering surface
 *The increase in memory consumption is (32 bits * object width) plus 24 bits * object width if using error diffusion */
 /*允许抖动渐变(以在有限的颜色深度显示上实现视觉平滑的颜色渐变)
 *LV_DITHER_GRADIENT 意味着分配对象渲染曲面的一条或多条线
 *如果使用误差扩散，内存消耗的增加是(32位*对象宽度)加上24位*对象宽度 */
#define LV_DITHER_GRADIENT      0
#if LV_DITHER_GRADIENT
    /*Add support for error diffusion dithering.
     *Error diffusion dithering gets a much better visual result, but implies more CPU consumption and memory when drawing.
     *The increase in memory consumption is (24 bits * object's width)*/
	/*添加对误差扩散抖动的支持
	*错误扩散抖动以获得更好的视觉效果，但在绘制时意味着更多的CPU消耗和RAM
     *内存消耗的增加是 (24 bits * 对象的宽度)*/
    #define LV_DITHER_ERROR_DIFFUSION   0
#endif

/*Maximum buffer size to allocate for rotation.
 *Only used if software rotation is enabled in the display driver.*
 *为旋转分配的最大缓冲区大小 
 *仅在显示驱动程序中启动软件旋转时使用.*/
#define LV_DISP_ROT_MAX_BUF (10*1024)

/*-------------
 * GPU
 *-----------*/

/*Use STM32's DMA2D (aka Chrom Art) GPU*/
#define LV_USE_GPU_STM32_DMA2D 0
#if LV_USE_GPU_STM32_DMA2D
    /*Must be defined to include path of CMSIS header of target processor
    e.g. "stm32f769xx.h" or "stm32f429xx.h"*/
	/*必须定义为包括目标处理器的CMSIS头文件的路径
    例如. "stm32f769xx.h" or "stm32f429xx.h"*/
    #define LV_GPU_DMA2D_CMSIS_INCLUDE
#endif

/*Use NXP's PXP GPU iMX RTxxx platforms*/
/*使用 NXP's PXP GPU iMX RTxxx 平台*/
#define LV_USE_GPU_NXP_PXP 0
#if LV_USE_GPU_NXP_PXP
    /*1: Add default bare metal and FreeRTOS interrupt handling routines for PXP (lv_gpu_nxp_pxp_osa.c)
    *   and call lv_gpu_nxp_pxp_init() automatically during lv_init(). Note that symbol SDK_OS_FREE_RTOS
    *   has to be defined in order to use FreeRTOS OSA, otherwise bare-metal implementation is selected.
    *0: lv_gpu_nxp_pxp_init() has to be called manually before lv_init()
    */
	/*1: 为PXP添加默认逻辑和FreeRTOS中断处理例程(lv_gpu_nxp_PXP_osa.c)
    *  在lv_init()期间自动调用lv_gpu_nxp_pxp_init(). 请注意符号SDK_OS_FREE_RTOS
    *   必须定义才能使用FreeRTOS OSA, 否则选择逻辑实现。
    *0: lv_gpu_nxp_pxp_init() 必须在lv_init()之前手动调用
    */
    #define LV_USE_GPU_NXP_PXP_AUTO_INIT 0
#endif

/*Use NXP's VG-Lite GPU iMX RTxxx platforms*/
#define LV_USE_GPU_NXP_VG_LITE 0

/*Use SDL renderer API*/
/*使用 SDL 渲染器 API*/
#define LV_USE_GPU_SDL 0
#if LV_USE_GPU_SDL
    #define LV_GPU_SDL_INCLUDE_PATH <SDL2/SDL.h>
    /*Texture cache size, 8MB by default*/
    #define LV_GPU_SDL_LRU_SIZE (1024 * 1024 * 8)
    /*Custom blend mode for mask drawing, disable if you need to link with older SDL2 lib*/
    #define LV_GPU_SDL_CUSTOM_BLEND_MODE (SDL_VERSION_ATLEAST(2, 0, 6))
#endif

/*-------------
 * Logging--日志
 *-----------*/

/*Enable the log module*/
/* 使能日志模型 */
#define LV_USE_LOG 0
#if LV_USE_LOG

    /*How important log should be added:--应添加的日志的重要性
    *LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information--提供详细信息的大量日志
    *LV_LOG_LEVEL_INFO        Log important events--记录重要事件
    *LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem--如果发生了不需要的事情，但没有引起问题，请记录
    *LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail--系统可能出现故障时的唯一关键问题
    *LV_LOG_LEVEL_USER        Only logs added by the user--仅用户添加的日志
    *LV_LOG_LEVEL_NONE        Do not log anything--不记录任何内容*/
    #define LV_LOG_LEVEL LV_LOG_LEVEL_INFO

    /*1: Print the log with 'printf';打印日志使用printf
    *0: User need to register a callback with `lv_log_register_print_cb()`*/
	/* 用户需要注册回调函数： lv_log_register_print_cb() */
    #define LV_LOG_PRINTF 1

    /*Enable/disable LV_LOG_TRACE in modules that produces a huge number of logs*/
	/* 在产生大量日志的模块中启用/禁用LV_LOG_TRACE */
    #define LV_LOG_TRACE_MEM        1
    #define LV_LOG_TRACE_TIMER      1
    #define LV_LOG_TRACE_INDEV      1
    #define LV_LOG_TRACE_DISP_REFR  1
    #define LV_LOG_TRACE_EVENT      1
    #define LV_LOG_TRACE_OBJ_CREATE 1
    #define LV_LOG_TRACE_LAYOUT     1
    #define LV_LOG_TRACE_ANIM       1

#endif  /*LV_USE_LOG*/

/*-------------
 * Asserts--断言
 *-----------*/

/*Enable asserts if an operation is failed or an invalid data is found.
 *If LV_USE_LOG is enabled an error message will be printed on failure*/
 /*如果操作失败或发现无效数据，则启用断言
 *如果启用LV_USE_LOG ，则故障时将打印错误消息*/
 #define LV_USE_ASSERT_NULL          1   /*Check if the parameter is NULL. (Very fast, recommended)--检查参数是否为空(非常快，推荐)*/
#define LV_USE_ASSERT_MALLOC        1   /*Checks is the memory is successfully allocated or no. (Very fast, recommended)--检查内存是否成功分配(非常快，推荐)*/
#define LV_USE_ASSERT_STYLE         0   /*Check if the styles are properly initialized. (Very fast, recommended)--检查样式是否正确初始化(非常快，推荐)*/
#define LV_USE_ASSERT_MEM_INTEGRITY 0   /*Check the integrity of `lv_mem` after critical operations. (Slow)--关键操作后检查lv_mem的完整性(慢)*/
#define LV_USE_ASSERT_OBJ           0   /*Check the object's type and existence (e.g. not deleted). (Slow)--检查对象的类型和存在(例如，未删除，(慢))*/

/*Add a custom handler when assert happens e.g. to restart the MCU*/
/*在发生断言时添加自定义处理程序，例如重新启动MCU*/
#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER while(1);   /*Halt by default--默认停止*/

/*-------------
 * Others--其他
 *-----------*/

/*1: Show CPU usage and FPS count*/
/*1: 显示CPU使用率和FPS计数*/
#define LV_USE_PERF_MONITOR 1
#if LV_USE_PERF_MONITOR
    #define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT
#endif

/*1: Show the used memory and the memory fragmentation
 * Requires LV_MEM_CUSTOM = 0*/
 /*1: 显示使用的内存和内存碎片
 * 需要 LV_MEM_CUSTOM = 0*/
#define LV_USE_MEM_MONITOR 0
#if LV_USE_MEM_MONITOR
    #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
#endif

/*1: Draw random colored rectangles over the redrawn areas*/
/*1: 在重新绘制区域上绘制随机彩色矩形*/
#define LV_USE_REFR_DEBUG 0

/*Change the built in (v)snprintf functions*/
/*更改内置的snprintf函数*/
#define LV_SPRINTF_CUSTOM 0
#if LV_SPRINTF_CUSTOM
    #define LV_SPRINTF_INCLUDE <stdio.h>
    #define lv_snprintf  snprintf
    #define lv_vsnprintf vsnprintf
#else   /*LV_SPRINTF_CUSTOM*/
    #define LV_SPRINTF_USE_FLOAT 1
#endif  /*LV_SPRINTF_CUSTOM*/

#define LV_USE_USER_DATA 0

/*Garbage Collector settings
 *Used if lvgl is bound to higher level language and the memory is managed by that language*/
 /*垃圾收集器设置
 *如果lvgl绑定到更高级别的语言，并且内存由该语言管理*/
#define LV_ENABLE_GC 0
#if LV_ENABLE_GC != 0
    #define LV_GC_INCLUDE "gc.h"  /*Include Garbage Collector related things--包括垃圾收集器相关的东西*/
#endif /*LV_ENABLE_GC*/

/*=====================
 *  COMPILER SETTINGS--编译器选项
 *====================*/

/*For big endian systems set to 1*/
/*对于大端系统，设置为1*/
#define LV_BIG_ENDIAN_SYSTEM 0

/*Define a custom attribute to `lv_tick_inc` function*/
/*将自定义属性定义为 `lv_tick_inc`*/
#define LV_ATTRIBUTE_TICK_INC

/*Define a custom attribute to `lv_timer_handler` function*/
/*将自定义属性定义为 `lv_timer_handler` 函数*/
#define LV_ATTRIBUTE_TIMER_HANDLER

/*Define a custom attribute to `lv_disp_flush_ready` function*/
/*将自定义属性定义为`lv_disp_flush_ready‘ 函数*/
#define LV_ATTRIBUTE_FLUSH_READY

/*Required alignment size for buffers*/
/*缓冲区所需的对齐大小*/
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 1

/*Will be added where memories needs to be aligned (with -Os data might not be aligned to boundary by default).
 * E.g. __attribute__((aligned(4)))*/
 /*将添加到需要对齐内存的位置(默认情况下，与-Os数据可能不对齐边界).
 * E.g. __attribute__((aligned(4)))*/
#define LV_ATTRIBUTE_MEM_ALIGN

/*Attribute to mark large constant arrays for example font's bitmaps*/
/*属性标记大常量数组，例如字体的位图*/
#define LV_ATTRIBUTE_LARGE_CONST

/*Compiler prefix for a big array declaration in RAM*/
/*RAM中大数组声明的编译器前缀*/
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY

/*Place performance critical functions into a faster memory (e.g RAM)*/
/*将性能关键功能放入速度更快的内存 (e.g RAM)*/
#define LV_ATTRIBUTE_FAST_MEM

/*Prefix variables that are used in GPU accelerated operations, often these need to be placed in RAM sections that are DMA accessible*/
/*GPU加速操作中使用的前缀变量，通常需要放置在DMA可访问的RAM部分中*/
#define LV_ATTRIBUTE_DMA

/*Export integer constant to binding. This macro is used with constants in the form of LV_<CONST> that
 *should also appear on LVGL binding API such as Micropython.*/
 /*将整数常量到处到绑定，该宏与 LV_<CONST> 形式的常量一起使用
 *还应出现在LVGL绑定的API上*/
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning /*The default value just prevents GCC warning*/

/*Extend the default -32k..32k coordinate range to -4M..4M by using int32_t for coordinates instead of int16_t*/
/*将默认的-32k..32k坐标范围扩展到-4m..4m,方法是使用int32_t而不是int16_t作为坐标*/
#define LV_USE_LARGE_COORD 0

/*==================
 *   FONT USAGE
 *===================*/

/*Montserrat fonts with ASCII range and some symbols using bpp = 4
 *https://fonts.google.com/specimen/Montserrat*/
/*Montserrat 字体，ASCII范围和一些符号使用bpp = 4
 *https://fonts.google.com/specimen/Montserrat*/
#define LV_FONT_MONTSERRAT_8  1
#define LV_FONT_MONTSERRAT_10 1
#define LV_FONT_MONTSERRAT_12 1
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 1
#define LV_FONT_MONTSERRAT_18 0
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 0
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0

/*Demonstrate special features*/
/*展示特色*/
#define LV_FONT_MONTSERRAT_12_SUBPX      0
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0  /*bpp = 3*/
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0  /*Hebrew, Arabic, Persian letters and all their forms*/
#define LV_FONT_SIMSUN_16_CJK            0  /*1000 most common CJK radicals*/

/*Pixel perfect monospace fonts*/
/*像素完美单空间字体*/
#define LV_FONT_UNSCII_8  0
#define LV_FONT_UNSCII_16 0

/*Optionally declare custom fonts here.
 *You can use these fonts as default font too and they will be available globally.
 *E.g. #define LV_FONT_CUSTOM_DECLARE   LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)*/
 /*可以选择在此处声明自定义字体
 *你也可以使用这些字体作为默认字体，他们将在全局可用
 *E.g. #define LV_FONT_CUSTOM_DECLARE   LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)*/
#define LV_FONT_CUSTOM_DECLARE LV_FONT_DECLARE(myFont36) \
							   LV_FONT_DECLARE(myFont20)
															   

/*Always set a default font*/
/*始终设置默认字体*/
#define LV_FONT_DEFAULT &lv_font_montserrat_24

/*Enable handling large font and/or fonts with a lot of characters.
 *The limit depends on the font size, font face and bpp.
 *Compiler error will be triggered if a font needs it.*/
 /*启用处理大字体/或包含大量字符的字体
 *限制取决与字体大小、字体面和bpp
 *如果字体需要，将触发编译器错误.*/
#define LV_FONT_FMT_TXT_LARGE 0

/*Enables/disables support for compressed fonts.*/
/*启用/禁用对压缩字体的支持*/
#define LV_USE_FONT_COMPRESSED 0

/*Enable subpixel rendering*/
/*启用子像素渲染*/
#define LV_USE_FONT_SUBPX 0
#if LV_USE_FONT_SUBPX
    /*Set the pixel order of the display. Physical order of RGB channels. Doesn't matter with "normal" fonts.*/
	/*设置显示器的像素顺序，RGB通道的物理顺序。普通字体没有关系.*/
    #define LV_FONT_SUBPX_BGR 0  /*0: RGB; 1:BGR order*/
#endif

/*=================
 *  TEXT SETTINGS--文本设置
 *=================*/

/**
 * Select a character encoding for strings.
 * Your IDE or editor should have the same character encoding
 * - LV_TXT_ENC_UTF8
 * - LV_TXT_ENC_ASCII
 */
 /**
 * 选择字符串的字符编码
 * IDE或编辑器应具有相同的字符编码
 * - LV_TXT_ENC_UTF8
 * - LV_TXT_ENC_ASCII
 */
#define LV_TXT_ENC 		LV_TXT_ENC_UTF8

/*Can break (wrap) texts on these chars*/
/*可以在这些字符上断开(换行)文本*/
#define LV_TXT_BREAK_CHARS " ,.;:-_"

/*If a word is at least this long, will break wherever "prettiest"
 *To disable, set to a value <= 0*/
 /*如果一个词至少这么长，他会在 "prettiest" 的地方断开
 *要禁用，请将值设置为 <= 0*/
#define LV_TXT_LINE_BREAK_LONG_LEN 0

/*Minimum number of characters in a long word to put on a line before a break.
 *Depends on LV_TXT_LINE_BREAK_LONG_LEN.*/
 /*在一个长单词中，在换行之前要放在一行中的最小字符数
 *取决于 LV_TXT_LINE_BREAK_LONG_LEN.*/
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3

/*Minimum number of characters in a long word to put on a line after a break.
 *Depends on LV_TXT_LINE_BREAK_LONG_LEN.*/
 /*一个长单词中的最小字符数，在中断后放在一行中
 *取决于 LV_TXT_LINE_BREAK_LONG_LEN.*/ 
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/*The control character to use for signalling text recoloring.*/
/*用于信号文本重新着色的控制字符*/
#define LV_TXT_COLOR_CMD "#"

/*Support bidirectional texts. Allows mixing Left-to-Right and Right-to-Left texts.
 *The direction will be processed according to the Unicode Bidirectional Algorithm:
 *https://www.w3.org/International/articles/inline-bidi-markup/uba-basics*/
 /*支持双向文本，允许混合从左到右和从右到左文本
 *方向将根据Unicode双向算法进行处理
 *https://www.w3.org/International/articles/inline-bidi-markup/uba-basics*/
#define LV_USE_BIDI 0
#if LV_USE_BIDI
    /*Set the default direction. Supported values:--设置默认方向，支持的值
    *`LV_BASE_DIR_LTR` Left-to-Right-左到右
    *`LV_BASE_DIR_RTL` Right-to-Left-右到左
    *`LV_BASE_DIR_AUTO` detect texts base direction--检测文本基本方向*/
    #define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
#endif

/*Enable Arabic/Persian processing
 *In these languages characters should be replaced with an other form based on their position in the text*/
 /*启用Arabic/Persian处理
 *在这些语言中，字符应根据其在文本中的位置用其他形式替换*/
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/*==================
 *  WIDGET USAGE--小部件使用
 *================*/

/*Documentation of the widgets: https://docs.lvgl.io/latest/en/html/widgets/index.html*/

#define LV_USE_ARC        1

#define LV_USE_ANIMIMG    1

#define LV_USE_BAR        1

#define LV_USE_BTN        1

#define LV_USE_BTNMATRIX  1

#define LV_USE_CANVAS     1

#define LV_USE_CHECKBOX   1

#define LV_USE_DROPDOWN   1   /*Requires: lv_label*/

#define LV_USE_IMG        1   /*Requires: lv_label*/

#define LV_USE_LABEL      1
#if LV_USE_LABEL
    #define LV_LABEL_TEXT_SELECTION 1 /*Enable selecting text of the label--启用选择标签的文本*/
    #define LV_LABEL_LONG_TXT_HINT 1  /*Store some extra info in labels to speed up drawing of very long texts*/
																	/*--在标签中存储一些额外信息，以加快绘制非常长的文本*/
#endif

#define LV_USE_LINE       1

#define LV_USE_ROLLER     1   /*Requires: lv_label*/
#if LV_USE_ROLLER
    #define LV_ROLLER_INF_PAGES 7 /*Number of extra "pages" when the roller is infinite--滚轮无限大时的额外页数*/
#endif

#define LV_USE_SLIDER     1   /*Requires: lv_bar*/

#define LV_USE_SWITCH     1

#define LV_USE_TEXTAREA   1   /*Requires: lv_label*/
#if LV_USE_TEXTAREA != 0
    #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500    /*ms*/
#endif

#define LV_USE_TABLE      1

/*==================
 * EXTRA COMPONENTS--特别零部件
 *==================*/

/*-----------
 * Widgets--小工具
 *----------*/
#define LV_USE_CALENDAR   1
#if LV_USE_CALENDAR
    #define LV_CALENDAR_WEEK_STARTS_MONDAY 0
    #if LV_CALENDAR_WEEK_STARTS_MONDAY
        #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Mo", "Tu", "We", "Th", "Fr", "Sa", "Su"}
    #else
        #define LV_CALENDAR_DEFAULT_DAY_NAMES {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"}
    #endif

    #define LV_CALENDAR_DEFAULT_MONTH_NAMES {"January", "February", "March",  "April", "May",  "June", "July", "August", "September", "October", "November", "December"}
    #define LV_USE_CALENDAR_HEADER_ARROW 1
    #define LV_USE_CALENDAR_HEADER_DROPDOWN 1
#endif  /*LV_USE_CALENDAR*/

#define LV_USE_CHART      1

#define LV_USE_COLORWHEEL 1

#define LV_USE_IMGBTN     1

#define LV_USE_KEYBOARD   1

#define LV_USE_LED        1

#define LV_USE_LIST       1

#define LV_USE_MENU       1

#define LV_USE_METER      1

#define LV_USE_MSGBOX     1

#define LV_USE_SPINBOX    1

#define LV_USE_SPINNER    1

#define LV_USE_TABVIEW    1

#define LV_USE_TILEVIEW   1

#define LV_USE_WIN        1

#define LV_USE_SPAN       1
#if LV_USE_SPAN
    /*A line text can contain maximum num of span descriptor--行文本可以包含最大跨度描述字符数 */
    #define LV_SPAN_SNIPPET_STACK_SIZE 64
#endif

/*-----------
 * Themes--主题
 *----------*/

/*A simple, impressive and very complete theme--一个简单、令人印象深刻且非常完整的主题*/
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT

    /*0: Light mode; 1: Dark mode*/
    #define LV_THEME_DEFAULT_DARK 0

    /*1: Enable grow on press*/
	/*1: 启用按需增长*/
    #define LV_THEME_DEFAULT_GROW 1

    /*Default transition time in [ms]*/
	/*默认转换时间 [ms]*/
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif /*LV_USE_THEME_DEFAULT*/

/*A very simple theme that is a good starting point for a custom theme*/
/*一个非常简单的主题，这是自定义主题的良好起点*/
#define LV_USE_THEME_BASIC 1

/*A theme designed for monochrome displays*/
/*为单色显示器设计的主题*/
#define LV_USE_THEME_MONO 1

/*-----------
 * Layouts--布局
 *----------*/

/*A layout similar to Flexbox in CSS.*/
/*类似于CSS中弹性框的布局.*/
#define LV_USE_FLEX 1

/*A layout similar to Grid in CSS.*/
/*类似CSS中网格的布局.*/
#define LV_USE_GRID 1

/*---------------------
 * 3rd party libraries--第三方库
 *--------------------*/

/*File system interfaces for common APIs */
/*通过API的文件系统接口 */

/*API for fopen, fread, etc*/
#define LV_USE_FS_STDIO 0
#if LV_USE_FS_STDIO
    #define LV_FS_STDIO_LETTER '\0'     /*Set an upper cased letter on which the drive will accessible (e.g. 'A')--设置可访问驱动器的大写字母，例如'A'*/
    #define LV_FS_STDIO_PATH ""         /*Set the working directory. File/directory paths will be appended to it.--设置工作目录，文件/目录路径将附加到他*/
    #define LV_FS_STDIO_CACHE_SIZE  0   /*>0 to cache this number of bytes in lv_fs_read()-->0在LV_FS_READ()中缓存此数字字节*/
#endif

/*API for open, read, etc*/
#define LV_USE_FS_POSIX 0
#if LV_USE_FS_POSIX
    #define LV_FS_POSIX_LETTER '\0'     /*Set an upper cased letter on which the drive will accessible (e.g. 'A')*/
    #define LV_FS_POSIX_PATH ""         /*Set the working directory. File/directory paths will be appended to it.*/
    #define LV_FS_POSIX_CACHE_SIZE  0   /*>0 to cache this number of bytes in lv_fs_read()*/
#endif

/*API for CreateFile, ReadFile, etc*/
#define LV_USE_FS_WIN32 0
#if LV_USE_FS_WIN32
    #define LV_FS_WIN32_LETTER  '\0'    /*Set an upper cased letter on which the drive will accessible (e.g. 'A')*/
    #define LV_FS_WIN32_PATH ""         /*Set the working directory. File/directory paths will be appended to it.*/
    #define LV_FS_WIN32_CACHE_SIZE 0    /*>0 to cache this number of bytes in lv_fs_read()*/
#endif

/*API for FATFS (needs to be added separately). Uses f_open, f_read, etc*/
#define LV_USE_FS_FATFS  1
#if LV_USE_FS_FATFS
#define LV_FS_FATFS_LETTER '0'     /*Set an upper cased letter on which the drive will accessible (e.g. 'A')*/
    #define LV_FS_FATFS_CACHE_SIZE 4*1024    /*>0 to cache this number of bytes in lv_fs_read()*/
#endif

/*PNG decoder library*/
/*PNG 解码器库*/
#define LV_USE_PNG 0

/*BMP decoder library*/
/*BMP 解码器库*/
#define LV_USE_BMP 0

/* JPG + split JPG decoder library.
 * Split JPG is a custom format optimized for embedded systems. */
 /* JPG + 拆分 JPG 解码器库.
 * Split JPG 是为嵌入式系统优化的定制格式. */
#define LV_USE_SJPG 0

/*GIF decoder library*/
/*GIF 解码器库*/
#define LV_USE_GIF 0

/*QR code library*/
#define LV_USE_QRCODE 0

/*FreeType library*/
/*自由类型库*/
#define LV_USE_FREETYPE 0
#if LV_USE_FREETYPE
    /*Memoryused by FreeType to cache characters [bytes] (-1: no caching)*/
	/* FreeType 用于缓存[bytes] (-1: 无缓存) 的内存 */
    #define LV_FREETYPE_CACHE_SIZE (16 * 1024)
    #if LV_FREETYPE_CACHE_SIZE >= 0
        /* 1: bitmap cache use the sbit cache, 0:bitmap cache use the image cache. */
        /* sbit cache:it is much more memory efficient for small bitmaps(font size < 256) */
        /* if font size >= 256, must be configured as image cache */
		/* 1: 位图缓存使用sbit缓存，0:位图缓存使用图像缓存. */
        /* sbit缓存，对于小位图(font size < 256)，他的内存效率更高 */
        /* 如果 font size >= 256, 则必须配置为图像缓存 */
        #define LV_FREETYPE_SBIT_CACHE 0
        /* Maximum number of opened FT_Face/FT_Size objects managed by this cache instance. */
        /* (0:use system defaults) */
        /* 此缓存实例管理的最大打开FT_Face/FT_Size 对象数. */
        /* (0:使用系统默认值) */		
        #define LV_FREETYPE_CACHE_FT_FACES 0
        #define LV_FREETYPE_CACHE_FT_SIZES 0
    #endif
#endif

/*Rlottie library*/
/*Rlottie 库*/	
#define LV_USE_RLOTTIE 0

/*FFmpeg library for image decoding and playing videos
 *Supports all major image formats so do not enable other image decoder with it*/
 /*用于图像解码和播放视频的FFmpeg 库
 *支持所有主要图像格式，因此不要启用其他图像解码器*/
#define LV_USE_FFMPEG  0
#if LV_USE_FFMPEG
    /*Dump input information to stderr*/
	/*将输入信息转储到stderr*/
    #define LV_FFMPEG_AV_DUMP_FORMAT 0
#endif

/*-----------
 * Others
 *----------*/

/*1: Enable API to take snapshot for object*/
/*1: 启用API以获取对象的快照*/
#define LV_USE_SNAPSHOT 0

/*1: Enable Monkey test*/
/*1: 启用猴子测试*/
#define LV_USE_MONKEY   0

/*1: Enable grid navigation*/
/*1: 启用网格导航*/
#define LV_USE_GRIDNAV  0

/*==================
* EXAMPLES
*==================*/

/*Enable the examples to be built with the library*/
/*启用要使用库构建的示例*/
#define LV_BUILD_EXAMPLES 1

/*===================
 * DEMO USAGE
 ====================*/

/*Show some widget. It might be required to increase `LV_MEM_SIZE` */
/*显示一些小部件，可能需要增加 `LV_MEM_SIZE` */
#define LV_USE_DEMO_WIDGETS        0
#if LV_USE_DEMO_WIDGETS
#define LV_DEMO_WIDGETS_SLIDESHOW  0
#endif

/*Demonstrate the usage of encoder and keyboard*/
/*演示解码器和键盘的使用*/
#define LV_USE_DEMO_KEYPAD_AND_ENCODER     0

/*Benchmark your system*/
/*对系统进行基准测试*/
#define LV_USE_DEMO_BENCHMARK   0

/*Stress test for LVGL*/
/*LVGL的压力测试*/
#define LV_USE_DEMO_STRESS      0

/*Music player demo*/
/*音乐播放器演示*/
#define LV_USE_DEMO_MUSIC       0
#if LV_USE_DEMO_MUSIC
# define LV_DEMO_MUSIC_SQUARE       0
# define LV_DEMO_MUSIC_LANDSCAPE    0
# define LV_DEMO_MUSIC_ROUND        0
# define LV_DEMO_MUSIC_LARGE        0
# define LV_DEMO_MUSIC_AUTO_PLAY    0
#endif

/*--END OF LV_CONF_H--*/

#endif /*LV_CONF_H*/

#endif /*End of "Content enable"--属性启用的结束*/
