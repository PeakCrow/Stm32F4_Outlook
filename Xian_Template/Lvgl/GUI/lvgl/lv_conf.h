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
* ��`lv_conf.h` ��������ļ�
 * 1. ��lvgl�ļ��з���һ��
 * 2. ���������κεط�
 *    - �궨���� `LV_CONF_INCLUDE_SIMPLE`
 *    - ���·����Ϊ����·��
 */
/* clang-format off */
#if 1 /*Set it to "1" to enable content*/

#ifndef LV_CONF_H
#define LV_CONF_H



#include <stdint.h>

/*====================
   COLOR SETTINGS--��ɫ����
 *====================*/

/*Color depth: 1 (1 byte per pixel), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
/*��ɫ���: 1 (ÿ����1�ֽ�), 8 (RGB332), 16 (RGB565), 32 (ARGB8888)*/
#define LV_COLOR_DEPTH 16

/*Swap the 2 bytes of RGB565 color. Useful if the display has an 8-bit interface (e.g. SPI)*/
/*����RGB565��ɫ��2�ֽ�. ��ʾ������8λ�ӿڣ�����SPI (e.g. SPI)*/
#define LV_COLOR_16_SWAP 0

/*Enable more complex drawing routines to manage screens transparency.
 *Can be used if the UI is above another layer, e.g. an OSD menu or video player.
 *Requires `LV_COLOR_DEPTH = 32` colors and the screen's `bg_opa` should be set to non LV_OPA_COVER value*/
 /*ʹ�ø����ӵĻ�ͼ������������Ļ͸����.
 *���UIλ����һ��֮�ϣ�����OSD�˵�������Ƶ�������������ʹ��
 *��ҪLV_COLOR_DEPTH=32��ɫ����Ļ��bg_opaӦ������Ϊ��LV_OPA_COVERֵ*/
#define LV_COLOR_SCREEN_TRANSP 0

/* Adjust color mix functions rounding. GPUs might calculate color mix (blending) differently.
 * 0: round down, 64: round up from x.75, 128: round up from half, 192: round up from x.25, 254: round up */
 /* ͨ���������������ɫ��Ϻ���.GPU�����Բ�ͬ�ķ�ʽ������ɫ���.
 * 0������ȡ֤, 64: ��x.75����ȡ��, 128: ��һ������ȡ��, 192: ��x.25����ȡ����, 254: ����ȡԲ */
#define LV_COLOR_MIX_ROUND_OFS (LV_COLOR_DEPTH == 32 ? 0: 128)

/*Images pixels with this color will not be drawn if they are chroma keyed)*/
/*ͼ�����ʹ��ɫ�ȼ��أ��򲻻���ƴ���ɫ������*/
#define LV_COLOR_CHROMA_KEY lv_color_hex(0x00ff00)         /*pure green*/

/*=========================
   MEMORY SETTINGS--�ڴ�����
 *=========================*/
/*1: use custom malloc/free, 0: use the built-in `lv_mem_alloc()` and `lv_mem_free()`*/
/*1: ʹ���Զ����malloc/free,0��ʹ�����õġ�lv_mem_alloc() �� lv_mem_free() */
#define LV_MEM_CUSTOM 0
#if LV_MEM_CUSTOM == 0
    /*Size of the memory available for `lv_mem_alloc()` in bytes (>= 2kB)*/
	/*�����ڡ�lv_mem_alloc()'������ڴ��С�����ֽ�Ϊ��λ�����ڵ���2Kb*/
    #define LV_MEM_SIZE (32U * 1024U)          /*[bytes]*/

    /*Set an address for the memory pool instead of allocating it as a normal array. Can be in external SRAM too.*/
	/*�����ڴ�صĵ�ַ�������ǽ������Ϊ��ͨ���飬Ҳ�������ⲿsram��.*/
    #define LV_MEM_ADR  0    /*0: unused*/
    /*Instead of an address give a memory allocator that will be called to get a memory pool for LVGL. E.g. my_malloc*/
	/*�����ַ����һ���ڴ�����������������������LVGL���ڴ�أ����� my_malloc*/
    #if LV_MEM_ADR == 0
        #undef LV_MEM_POOL_INCLUDE 
        #undef LV_MEM_POOL_ALLOC   
    #endif

#else       /*LV_MEM_CUSTOM*/
    #define LV_MEM_CUSTOM_INCLUDE  <stdlib.h>	//<stdlib.h>    "malloc.h" /*Header for the dynamic memory function*/
    #define LV_MEM_CUSTOM_ALLOC   malloc	//malloc  lv_mymalloc
    #define LV_MEM_CUSTOM_FREE    free		//free  lv_myfree
    #define LV_MEM_CUSTOM_REALLOC realloc//realloc  lv_myrealloc
#endif     /*LV_MEM_CUSTOM*/

/*Number of the intermediate memory buffer used during rendering and other internal processing mechanisms.
 *You will see an error log message if there wasn't enough buffers. */
 /*��Ⱦ��������������ڼ�ʹ�õ��м��ڴ滺����������.
 *������������㣬�㽫��������������Ϣ. */
#define LV_MEM_BUF_MAX_NUM 16

/*Use the standard `memcpy` and `memset` instead of LVGL's own functions. (Might or might not be faster).*/
/*ʹ�ñ�׼�� `memcpy` and `memset` ��������LVGL�Լ��Ĺ���(���ܸ��죬Ҳ���ܲ������).*/
#define LV_MEMCPY_MEMSET_STD 0

/*====================
   HAL SETTINGS--HAL����
 *====================*/

/*Default display refresh period. LVG will redraw changed areas with this period time*/
/* Ĭ����ʾˢ�����ڣ�LVGL���ڴ��ڼ����»��Ƹ������� */
#define LV_DISP_DEF_REFR_PERIOD 10      /*[ms]*/

/*Input device read period in milliseconds*/
/*�����豸��ȡ����*/
#define LV_INDEV_DEF_READ_PERIOD 10     /*[ms]*/

/*Use a custom tick source that tells the elapsed time in milliseconds.
 *It removes the need to manually update the tick with `lv_tick_inc()`)*/
 /*ʹ���Ժ���Ϊ��λ��ʾ����ʱ����Զ���̶�Դ.
 *��������ʹ��`lv_tick_inc()`�ֶ������źŵ���Ҫ*/
#define LV_TICK_CUSTOM 1
#if LV_TICK_CUSTOM
    #define LV_TICK_CUSTOM_INCLUDE "sys.h"         /*Header for the system time function*/
    #define LV_TICK_CUSTOM_SYS_TIME_EXPR (tx_time_get())    /*Expression evaluating to current system time in ms*/
#endif   /*LV_TICK_CUSTOM*/

/*Default Dot Per Inch. Used to initialize default sizes such as widgets sized, style paddings.
 *(Not so important, you can adjust it to modify default sizes and spaces)*/
 /*Ĭ�����ص��ÿӢ���С�����ڳ�ʼ��Ĭ�ϴ�С����С������С����ʽ���.
 *��̫��Ҫ������Ե��������޸�Ĭ�ϴ�С�Ϳռ�*/
#define LV_DPI_DEF 130     /*[px/inch]*/

/*=======================
 * FEATURE CONFIGURATION--��������
 *=======================*/

/*-------------
 * Drawing--ͼ��
 *-----------*/

/*Enable complex draw engine.
 *Required to draw shadow, gradient, rounded corners, circles, arc, skew lines, image transformations or any masks*/
 /*�������ӻ�ͼ����
 *��Ҫ������Ӱ�����䡢Բ�ǡ�Բ��Բ����б�ߡ�ͼ��任���κ�����*/
#define LV_DRAW_COMPLEX 1
#if LV_DRAW_COMPLEX != 0

    /*Allow buffering some shadow calculation.
    *LV_SHADOW_CACHE_SIZE is the max. shadow size to buffer, where shadow size is `shadow_width + radius`
    *Caching has LV_SHADOW_CACHE_SIZE^2 RAM cost*/
	/*������һЩ��Ӱ����.
    *LV_SHADOW_CACHE_SIZE �ǻ������������Ӱ��С��������Ӱ��СΪ����Ӱ���+�뾶`
    *�������LV_SHADOW_CACHE_SIZE^2 ��С��RAM�ɱ�*/
    #define LV_SHADOW_CACHE_SIZE 0

    /* Set number of maximally cached circle data.
    * The circumference of 1/4 circle are saved for anti-aliasing
    * radius * 4 bytes are used per circle (the most often used radiuses are saved)
    * 0: to disable caching */
	/*������󻺴��ԲȦ������
    * ����1/4���ܳ��Խ��п����
    * ���뾶��ÿ��ԲȦʹ��4�ֽ�(������õİ뾶)
    * 0�����û���*/
    #define LV_CIRCLE_CACHE_SIZE 4
#endif /*LV_DRAW_COMPLEX*/

/*Default image cache size. Image caching keeps the images opened.
 *If only the built-in image formats are used there is no real advantage of caching. (I.e. if no new image decoder is added)
 *With complex image decoders (e.g. PNG or JPG) caching can save the continuous open/decode of images.
 *However the opened images might consume additional RAM.
 *0: to disable caching*/
 /*Ĭ��ͼ�񻺴��С��ͼ�񻺴�ʹͼ�񱣳ִ�״̬
 *���ֻʹ������ͼ���ʽ���򻺴�û�����������ƣ�(�������û������µ�ͼ�������)
 *���ڸ��ӵ�ͼ�������(����PNG����JPG)��������Ա���ͼ���������/����
 *���ǣ��򿪵�ӳ����ܻ����Ķ����RAM
 *0: ���û���*/
#define LV_IMG_CACHE_DEF_SIZE   0

/*Number of stops allowed per gradient. Increase this to allow more stops.
 *This adds (sizeof(lv_color_t) + 1) bytes per additional stop*/
 /*ÿ���ݶ������ֹͣ�������Ӹ�ֵ����������ֹͣ
 *ÿ����һ��վ�㣬������(sizeof(lv_color_t) + 1���ֽ�*/
#define LV_GRADIENT_MAX_STOPS       2

/*Default gradient buffer size.
 *When LVGL calculates the gradient "maps" it can save them into a cache to avoid calculating them again.
 *LV_GRAD_CACHE_DEF_SIZE sets the size of this cache in bytes.
 *If the cache is too small the map will be allocated only while it's required for the drawing.
 *0 mean no caching.*/
 /*Ĭ�Ͻ��仺������С
 *��LVGL���㽥����ͼʱ�������Խ����Ǳ����ڻ����У��Ա����ٴμ�������
 *LV_GRAD_CACHE_DEF_SIZE ���ֽ�Ϊ��λ���ô˻���Ĵ�С.
 *�������̫С�������ͼ����Ҫʱ������ͼ
 *0 ��ʾû�л���.*/
#define LV_GRAD_CACHE_DEF_SIZE      0

/*Allow dithering the gradients (to achieve visual smooth color gradients on limited color depth display)
 *LV_DITHER_GRADIENT implies allocating one or two more lines of the object's rendering surface
 *The increase in memory consumption is (32 bits * object width) plus 24 bits * object width if using error diffusion */
 /*����������(�������޵���ɫ�����ʾ��ʵ���Ӿ�ƽ������ɫ����)
 *LV_DITHER_GRADIENT ��ζ�ŷ��������Ⱦ�����һ���������
 *���ʹ�������ɢ���ڴ����ĵ�������(32λ*������)����24λ*������ */
#define LV_DITHER_GRADIENT      0
#if LV_DITHER_GRADIENT
    /*Add support for error diffusion dithering.
     *Error diffusion dithering gets a much better visual result, but implies more CPU consumption and memory when drawing.
     *The increase in memory consumption is (24 bits * object's width)*/
	/*��Ӷ������ɢ������֧��
	*������ɢ�����Ի�ø��õ��Ӿ�Ч�������ڻ���ʱ��ζ�Ÿ����CPU���ĺ�RAM
     *�ڴ����ĵ������� (24 bits * ����Ŀ��)*/
    #define LV_DITHER_ERROR_DIFFUSION   0
#endif

/*Maximum buffer size to allocate for rotation.
 *Only used if software rotation is enabled in the display driver.*
 *Ϊ��ת�������󻺳�����С 
 *������ʾ�������������������תʱʹ��.*/
#define LV_DISP_ROT_MAX_BUF (10*1024)

/*-------------
 * GPU
 *-----------*/

/*Use STM32's DMA2D (aka Chrom Art) GPU*/
#define LV_USE_GPU_STM32_DMA2D 0
#if LV_USE_GPU_STM32_DMA2D
    /*Must be defined to include path of CMSIS header of target processor
    e.g. "stm32f769xx.h" or "stm32f429xx.h"*/
	/*���붨��Ϊ����Ŀ�괦������CMSISͷ�ļ���·��
    ����. "stm32f769xx.h" or "stm32f429xx.h"*/
    #define LV_GPU_DMA2D_CMSIS_INCLUDE
#endif

/*Use NXP's PXP GPU iMX RTxxx platforms*/
/*ʹ�� NXP's PXP GPU iMX RTxxx ƽ̨*/
#define LV_USE_GPU_NXP_PXP 0
#if LV_USE_GPU_NXP_PXP
    /*1: Add default bare metal and FreeRTOS interrupt handling routines for PXP (lv_gpu_nxp_pxp_osa.c)
    *   and call lv_gpu_nxp_pxp_init() automatically during lv_init(). Note that symbol SDK_OS_FREE_RTOS
    *   has to be defined in order to use FreeRTOS OSA, otherwise bare-metal implementation is selected.
    *0: lv_gpu_nxp_pxp_init() has to be called manually before lv_init()
    */
	/*1: ΪPXP���Ĭ���߼���FreeRTOS�жϴ�������(lv_gpu_nxp_PXP_osa.c)
    *  ��lv_init()�ڼ��Զ�����lv_gpu_nxp_pxp_init(). ��ע�����SDK_OS_FREE_RTOS
    *   ���붨�����ʹ��FreeRTOS OSA, ����ѡ���߼�ʵ�֡�
    *0: lv_gpu_nxp_pxp_init() ������lv_init()֮ǰ�ֶ�����
    */
    #define LV_USE_GPU_NXP_PXP_AUTO_INIT 0
#endif

/*Use NXP's VG-Lite GPU iMX RTxxx platforms*/
#define LV_USE_GPU_NXP_VG_LITE 0

/*Use SDL renderer API*/
/*ʹ�� SDL ��Ⱦ�� API*/
#define LV_USE_GPU_SDL 0
#if LV_USE_GPU_SDL
    #define LV_GPU_SDL_INCLUDE_PATH <SDL2/SDL.h>
    /*Texture cache size, 8MB by default*/
    #define LV_GPU_SDL_LRU_SIZE (1024 * 1024 * 8)
    /*Custom blend mode for mask drawing, disable if you need to link with older SDL2 lib*/
    #define LV_GPU_SDL_CUSTOM_BLEND_MODE (SDL_VERSION_ATLEAST(2, 0, 6))
#endif

/*-------------
 * Logging--��־
 *-----------*/

/*Enable the log module*/
/* ʹ����־ģ�� */
#define LV_USE_LOG 0
#if LV_USE_LOG

    /*How important log should be added:--Ӧ��ӵ���־����Ҫ��
    *LV_LOG_LEVEL_TRACE       A lot of logs to give detailed information--�ṩ��ϸ��Ϣ�Ĵ�����־
    *LV_LOG_LEVEL_INFO        Log important events--��¼��Ҫ�¼�
    *LV_LOG_LEVEL_WARN        Log if something unwanted happened but didn't cause a problem--��������˲���Ҫ�����飬��û���������⣬���¼
    *LV_LOG_LEVEL_ERROR       Only critical issue, when the system may fail--ϵͳ���ܳ��ֹ���ʱ��Ψһ�ؼ�����
    *LV_LOG_LEVEL_USER        Only logs added by the user--���û���ӵ���־
    *LV_LOG_LEVEL_NONE        Do not log anything--����¼�κ�����*/
    #define LV_LOG_LEVEL      LV_LOG_LEVEL_TRACE

    /*1: Print the log with 'printf';��ӡ��־ʹ��printf
    *0: User need to register a callback with `lv_log_register_print_cb()`*/
	/* �û���Ҫע��ص������� lv_log_register_print_cb() */
    #define LV_LOG_PRINTF 1

    /*Enable/disable LV_LOG_TRACE in modules that produces a huge number of logs*/
	/* �ڲ���������־��ģ��������/����LV_LOG_TRACE */
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
 * Asserts--����
 *-----------*/

/*Enable asserts if an operation is failed or an invalid data is found.
 *If LV_USE_LOG is enabled an error message will be printed on failure*/
 /*�������ʧ�ܻ�����Ч���ݣ������ö���
 *�������LV_USE_LOG �������ʱ����ӡ������Ϣ*/
#define LV_USE_ASSERT_NULL          1   /*Check if the parameter is NULL. (Very fast, recommended)--�������Ƿ�Ϊ��(�ǳ��죬�Ƽ�)*/
#define LV_USE_ASSERT_MALLOC        1   /*Checks is the memory is successfully allocated or no. (Very fast, recommended)--����ڴ��Ƿ�ɹ�����(�ǳ��죬�Ƽ�)*/
#define LV_USE_ASSERT_STYLE         1   /*Check if the styles are properly initialized. (Very fast, recommended)--�����ʽ�Ƿ���ȷ��ʼ��(�ǳ��죬�Ƽ�)*/
#define LV_USE_ASSERT_MEM_INTEGRITY 0   /*Check the integrity of `lv_mem` after critical operations. (Slow)--�ؼ���������lv_mem��������(��)*/
#define LV_USE_ASSERT_OBJ           0   /*Check the object's type and existence (e.g. not deleted). (Slow)--����������ͺʹ���(���磬δɾ����(��))*/

/*Add a custom handler when assert happens e.g. to restart the MCU*/
/*�ڷ�������ʱ����Զ��崦�����������������MCU*/
#define LV_ASSERT_HANDLER_INCLUDE <stdint.h>
#define LV_ASSERT_HANDLER while(1);   /*Halt by default--Ĭ��ֹͣ*/

/*-------------
 * Others--����
 *-----------*/

/*1: Show CPU usage and FPS count*/
/*1: ��ʾCPUʹ���ʺ�FPS����*/
#define LV_USE_PERF_MONITOR 1
#if LV_USE_PERF_MONITOR
    #define LV_USE_PERF_MONITOR_POS LV_ALIGN_BOTTOM_RIGHT
#endif

/*1: Show the used memory and the memory fragmentation
 * Requires LV_MEM_CUSTOM = 0*/
 /*1: ��ʾʹ�õ��ڴ���ڴ���Ƭ
 * ��Ҫ LV_MEM_CUSTOM = 0*/
#define LV_USE_MEM_MONITOR 1
#if LV_USE_MEM_MONITOR
    #define LV_USE_MEM_MONITOR_POS LV_ALIGN_BOTTOM_LEFT
#endif

/*1: Draw random colored rectangles over the redrawn areas*/
/*1: �����»��������ϻ��������ɫ����*/
#define LV_USE_REFR_DEBUG 0

/*Change the built in (v)snprintf functions*/
/*�������õ�snprintf����*/
#define LV_SPRINTF_CUSTOM 0
#if LV_SPRINTF_CUSTOM
    #define LV_SPRINTF_INCLUDE <stdio.h>
    #define lv_snprintf  snprintf
    #define lv_vsnprintf vsnprintf
#else   /*LV_SPRINTF_CUSTOM*/
    #define LV_SPRINTF_USE_FLOAT 1
#endif  /*LV_SPRINTF_CUSTOM*/

#define LV_USE_USER_DATA 1

/*Garbage Collector settings
 *Used if lvgl is bound to higher level language and the memory is managed by that language*/
 /*�����ռ�������
 *���lvgl�󶨵����߼�������ԣ������ڴ��ɸ����Թ���*/
#define LV_ENABLE_GC 0
#if LV_ENABLE_GC != 0
    #define LV_GC_INCLUDE "gc.h"  /*Include Garbage Collector related things--���������ռ�����صĶ���*/
#endif /*LV_ENABLE_GC*/

/*=====================
 *  COMPILER SETTINGS--������ѡ��
 *====================*/

/*For big endian systems set to 1*/
/*���ڴ��ϵͳ������Ϊ1*/
#define LV_BIG_ENDIAN_SYSTEM 0

/*Define a custom attribute to `lv_tick_inc` function*/
/*���Զ������Զ���Ϊ `lv_tick_inc`*/
#define LV_ATTRIBUTE_TICK_INC

/*Define a custom attribute to `lv_timer_handler` function*/
/*���Զ������Զ���Ϊ `lv_timer_handler` ����*/
#define LV_ATTRIBUTE_TIMER_HANDLER

/*Define a custom attribute to `lv_disp_flush_ready` function*/
/*���Զ������Զ���Ϊ`lv_disp_flush_ready�� ����*/
#define LV_ATTRIBUTE_FLUSH_READY

/*Required alignment size for buffers*/
/*����������Ķ����С*/
#define LV_ATTRIBUTE_MEM_ALIGN_SIZE 1

/*Will be added where memories needs to be aligned (with -Os data might not be aligned to boundary by default).
 * E.g. __attribute__((aligned(4)))*/
 /*����ӵ���Ҫ�����ڴ��λ��(Ĭ������£���-Os���ݿ��ܲ�����߽�).
 * E.g. __attribute__((aligned(4)))*/
#define LV_ATTRIBUTE_MEM_ALIGN

/*Attribute to mark large constant arrays for example font's bitmaps*/
/*���Ա�Ǵ������飬���������λͼ*/
#define LV_ATTRIBUTE_LARGE_CONST

/*Compiler prefix for a big array declaration in RAM*/
/*RAM�д����������ı�����ǰ׺*/
#define LV_ATTRIBUTE_LARGE_RAM_ARRAY

/*Place performance critical functions into a faster memory (e.g RAM)*/
/*�����ܹؼ����ܷ����ٶȸ�����ڴ� (e.g RAM)*/
#define LV_ATTRIBUTE_FAST_MEM

/*Prefix variables that are used in GPU accelerated operations, often these need to be placed in RAM sections that are DMA accessible*/
/*GPU���ٲ�����ʹ�õ�ǰ׺������ͨ����Ҫ������DMA�ɷ��ʵ�RAM������*/
#define LV_ATTRIBUTE_DMA

/*Export integer constant to binding. This macro is used with constants in the form of LV_<CONST> that
 *should also appear on LVGL binding API such as Micropython.*/
 /*�����������������󶨣��ú��� LV_<CONST> ��ʽ�ĳ���һ��ʹ��
 *��Ӧ������LVGL�󶨵�API��*/
#define LV_EXPORT_CONST_INT(int_value) struct _silence_gcc_warning /*The default value just prevents GCC warning*/

/*Extend the default -32k..32k coordinate range to -4M..4M by using int32_t for coordinates instead of int16_t*/
/*��Ĭ�ϵ�-32k..32k���귶Χ��չ��-4m..4m,������ʹ��int32_t������int16_t��Ϊ����*/
#define LV_USE_LARGE_COORD 0

/*==================
 *   FONT USAGE
 *===================*/

/*Montserrat fonts with ASCII range and some symbols using bpp = 4
 *https://fonts.google.com/specimen/Montserrat*/
/*Montserrat ���壬ASCII��Χ��һЩ����ʹ��bpp = 4
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
/*չʾ��ɫ*/
#define LV_FONT_MONTSERRAT_12_SUBPX      0
#define LV_FONT_MONTSERRAT_28_COMPRESSED 0  /*bpp = 3*/
#define LV_FONT_DEJAVU_16_PERSIAN_HEBREW 0  /*Hebrew, Arabic, Persian letters and all their forms*/
#define LV_FONT_SIMSUN_16_CJK            0  /*1000 most common CJK radicals*/

/*Pixel perfect monospace fonts*/
/*�����������ռ�����*/
#define LV_FONT_UNSCII_8  0
#define LV_FONT_UNSCII_16 0

/*Optionally declare custom fonts here.
 *You can use these fonts as default font too and they will be available globally.
 *E.g. #define LV_FONT_CUSTOM_DECLARE   LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)*/
 /*����ѡ���ڴ˴������Զ�������
 *��Ҳ����ʹ����Щ������ΪĬ�����壬���ǽ���ȫ�ֿ���
 *E.g. #define LV_FONT_CUSTOM_DECLARE   LV_FONT_DECLARE(my_font_1) LV_FONT_DECLARE(my_font_2)*/
#define LV_FONT_CUSTOM_DECLARE LV_FONT_DECLARE(myFont36) \
							   LV_FONT_DECLARE(myFont20)
															   

/*Always set a default font*/
/*ʼ������Ĭ������*/
#define LV_FONT_DEFAULT &myFont20

/*Enable handling large font and/or fonts with a lot of characters.
 *The limit depends on the font size, font face and bpp.
 *Compiler error will be triggered if a font needs it.*/
 /*���ô��������/����������ַ�������
 *����ȡ���������С���������bpp
 *���������Ҫ������������������.*/
#define LV_FONT_FMT_TXT_LARGE 0

/*Enables/disables support for compressed fonts.*/
/*����/���ö�ѹ�������֧��*/
#define LV_USE_FONT_COMPRESSED 0

/*Enable subpixel rendering*/
/*������������Ⱦ*/
#define LV_USE_FONT_SUBPX 0
#if LV_USE_FONT_SUBPX
    /*Set the pixel order of the display. Physical order of RGB channels. Doesn't matter with "normal" fonts.*/
	/*������ʾ��������˳��RGBͨ��������˳����ͨ����û�й�ϵ.*/
    #define LV_FONT_SUBPX_BGR 0  /*0: RGB; 1:BGR order*/
#endif

/*=================
 *  TEXT SETTINGS--�ı�����
 *=================*/

/**
 * Select a character encoding for strings.
 * Your IDE or editor should have the same character encoding
 * - LV_TXT_ENC_UTF8
 * - LV_TXT_ENC_ASCII
 */
 /**
 * ѡ���ַ������ַ�����
 * IDE��༭��Ӧ������ͬ���ַ�����
 * - LV_TXT_ENC_UTF8
 * - LV_TXT_ENC_ASCII
 */
#define LV_TXT_ENC 		LV_TXT_ENC_UTF8

/*Can break (wrap) texts on these chars*/
/*��������Щ�ַ��϶Ͽ�(����)�ı�*/
#define LV_TXT_BREAK_CHARS " ,.;:-_"

/*If a word is at least this long, will break wherever "prettiest"
 *To disable, set to a value <= 0*/
 /*���һ����������ô���������� "prettiest" �ĵط��Ͽ�
 *Ҫ���ã��뽫ֵ����Ϊ <= 0*/
#define LV_TXT_LINE_BREAK_LONG_LEN 0

/*Minimum number of characters in a long word to put on a line before a break.
 *Depends on LV_TXT_LINE_BREAK_LONG_LEN.*/
 /*��һ���������У��ڻ���֮ǰҪ����һ���е���С�ַ���
 *ȡ���� LV_TXT_LINE_BREAK_LONG_LEN.*/
#define LV_TXT_LINE_BREAK_LONG_PRE_MIN_LEN 3

/*Minimum number of characters in a long word to put on a line after a break.
 *Depends on LV_TXT_LINE_BREAK_LONG_LEN.*/
 /*һ���������е���С�ַ��������жϺ����һ����
 *ȡ���� LV_TXT_LINE_BREAK_LONG_LEN.*/ 
#define LV_TXT_LINE_BREAK_LONG_POST_MIN_LEN 3

/*The control character to use for signalling text recoloring.*/
/*�����ź��ı�������ɫ�Ŀ����ַ�*/
#define LV_TXT_COLOR_CMD "#"

/*Support bidirectional texts. Allows mixing Left-to-Right and Right-to-Left texts.
 *The direction will be processed according to the Unicode Bidirectional Algorithm:
 *https://www.w3.org/International/articles/inline-bidi-markup/uba-basics*/
 /*֧��˫���ı��������ϴ����Һʹ��ҵ����ı�
 *���򽫸���Unicode˫���㷨���д���
 *https://www.w3.org/International/articles/inline-bidi-markup/uba-basics*/
#define LV_USE_BIDI 0
#if LV_USE_BIDI
    /*Set the default direction. Supported values:--����Ĭ�Ϸ���֧�ֵ�ֵ
    *`LV_BASE_DIR_LTR` Left-to-Right-����
    *`LV_BASE_DIR_RTL` Right-to-Left-�ҵ���
    *`LV_BASE_DIR_AUTO` detect texts base direction--����ı���������*/
    #define LV_BIDI_BASE_DIR_DEF LV_BASE_DIR_AUTO
#endif

/*Enable Arabic/Persian processing
 *In these languages characters should be replaced with an other form based on their position in the text*/
 /*����Arabic/Persian����
 *����Щ�����У��ַ�Ӧ���������ı��е�λ����������ʽ�滻*/
#define LV_USE_ARABIC_PERSIAN_CHARS 0

/*==================
 *  WIDGET USAGE--С����ʹ��
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
    #define LV_LABEL_TEXT_SELECTION 1 /*Enable selecting text of the label--����ѡ���ǩ���ı�*/
    #define LV_LABEL_LONG_TXT_HINT 1  /*Store some extra info in labels to speed up drawing of very long texts*/
																	/*--�ڱ�ǩ�д洢һЩ������Ϣ���Լӿ���Ʒǳ������ı�*/
#endif

#define LV_USE_LINE       1

#define LV_USE_ROLLER     1   /*Requires: lv_label*/
#if LV_USE_ROLLER
    #define LV_ROLLER_INF_PAGES 7 /*Number of extra "pages" when the roller is infinite--�������޴�ʱ�Ķ���ҳ��*/
#endif

#define LV_USE_SLIDER     1   /*Requires: lv_bar*/

#define LV_USE_SWITCH     1

#define LV_USE_TEXTAREA   1   /*Requires: lv_label*/
#if LV_USE_TEXTAREA != 0
    #define LV_TEXTAREA_DEF_PWD_SHOW_TIME 1500    /*ms*/
#endif

#define LV_USE_TABLE      1

/*==================
 * EXTRA COMPONENTS--�ر��㲿��
 *==================*/

/*-----------
 * Widgets--С����
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
    /*A line text can contain maximum num of span descriptor--���ı����԰�������������ַ��� */
    #define LV_SPAN_SNIPPET_STACK_SIZE 64
#endif

/*-----------
 * Themes--����
 *----------*/

/*A simple, impressive and very complete theme--һ���򵥡�����ӡ������ҷǳ�����������*/
#define LV_USE_THEME_DEFAULT 1
#if LV_USE_THEME_DEFAULT

    /*0: Light mode; 1: Dark mode*/
    #define LV_THEME_DEFAULT_DARK 0

    /*1: Enable grow on press*/
	/*1: ���ð�������*/
    #define LV_THEME_DEFAULT_GROW 1

    /*Default transition time in [ms]*/
	/*Ĭ��ת��ʱ�� [ms]*/
    #define LV_THEME_DEFAULT_TRANSITION_TIME 80
#endif /*LV_USE_THEME_DEFAULT*/

/*A very simple theme that is a good starting point for a custom theme*/
/*һ���ǳ��򵥵����⣬�����Զ���������������*/
#define LV_USE_THEME_BASIC 1

/*A theme designed for monochrome displays*/
/*Ϊ��ɫ��ʾ����Ƶ�����*/
#define LV_USE_THEME_MONO 1

/*-----------
 * Layouts--����
 *----------*/

/*A layout similar to Flexbox in CSS.*/
/*������CSS�е��Կ�Ĳ���.*/
#define LV_USE_FLEX 1

/*A layout similar to Grid in CSS.*/
/*����CSS������Ĳ���.*/
#define LV_USE_GRID 1

/*---------------------
 * 3rd party libraries--��������
 *--------------------*/

/*File system interfaces for common APIs */
/*ͨ��API���ļ�ϵͳ�ӿ� */

/*API for fopen, fread, etc*/
#define LV_USE_FS_STDIO 0
#if LV_USE_FS_STDIO
    #define LV_FS_STDIO_LETTER '\0'     /*Set an upper cased letter on which the drive will accessible (e.g. 'A')--���ÿɷ����������Ĵ�д��ĸ������'A'*/
    #define LV_FS_STDIO_PATH ""         /*Set the working directory. File/directory paths will be appended to it.--���ù���Ŀ¼���ļ�/Ŀ¼·�������ӵ���*/
    #define LV_FS_STDIO_CACHE_SIZE  0   /*>0 to cache this number of bytes in lv_fs_read()-->0��LV_FS_READ()�л���������ֽ�*/
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
/*PNG ��������*/
#define LV_USE_PNG 0

/*BMP decoder library*/
/*BMP ��������*/
#define LV_USE_BMP 0

/* JPG + split JPG decoder library.
 * Split JPG is a custom format optimized for embedded systems. */
 /* JPG + ��� JPG ��������.
 * Split JPG ��ΪǶ��ʽϵͳ�Ż��Ķ��Ƹ�ʽ. */
#define LV_USE_SJPG 0

/*GIF decoder library*/
/*GIF ��������*/
#define LV_USE_GIF 0

/*QR code library*/
#define LV_USE_QRCODE 0

/*FreeType library*/
/*�������Ϳ�*/
#define LV_USE_FREETYPE 0
#if LV_USE_FREETYPE
    /*Memoryused by FreeType to cache characters [bytes] (-1: no caching)*/
	/* FreeType ���ڻ���[bytes] (-1: �޻���) ���ڴ� */
    #define LV_FREETYPE_CACHE_SIZE (16 * 1024)
    #if LV_FREETYPE_CACHE_SIZE >= 0
        /* 1: bitmap cache use the sbit cache, 0:bitmap cache use the image cache. */
        /* sbit cache:it is much more memory efficient for small bitmaps(font size < 256) */
        /* if font size >= 256, must be configured as image cache */
		/* 1: λͼ����ʹ��sbit���棬0:λͼ����ʹ��ͼ�񻺴�. */
        /* sbit���棬����Сλͼ(font size < 256)�������ڴ�Ч�ʸ��� */
        /* ��� font size >= 256, ���������Ϊͼ�񻺴� */
        #define LV_FREETYPE_SBIT_CACHE 0
        /* Maximum number of opened FT_Face/FT_Size objects managed by this cache instance. */
        /* (0:use system defaults) */
        /* �˻���ʵ�����������FT_Face/FT_Size ������. */
        /* (0:ʹ��ϵͳĬ��ֵ) */		
        #define LV_FREETYPE_CACHE_FT_FACES 0
        #define LV_FREETYPE_CACHE_FT_SIZES 0
    #endif
#endif

/*Rlottie library*/
/*Rlottie ��*/	
#define LV_USE_RLOTTIE 0

/*FFmpeg library for image decoding and playing videos
 *Supports all major image formats so do not enable other image decoder with it*/
 /*����ͼ�����Ͳ�����Ƶ��FFmpeg ��
 *֧��������Ҫͼ���ʽ����˲�Ҫ��������ͼ�������*/
#define LV_USE_FFMPEG  0
#if LV_USE_FFMPEG
    /*Dump input information to stderr*/
	/*��������Ϣת����stderr*/
    #define LV_FFMPEG_AV_DUMP_FORMAT 0
#endif

/*-----------
 * Others
 *----------*/

/*1: Enable API to take snapshot for object*/
/*1: ����API�Ի�ȡ����Ŀ���*/
#define LV_USE_SNAPSHOT 0

/*1: Enable Monkey test*/
/*1: ���ú��Ӳ���*/
#define LV_USE_MONKEY   0

/*1: Enable grid navigation*/
/*1: �������񵼺�*/
#define LV_USE_GRIDNAV  0

/*==================
* EXAMPLES
*==================*/

/*Enable the examples to be built with the library*/
/*����Ҫʹ�ÿ⹹����ʾ��*/
#define LV_BUILD_EXAMPLES 1

/*===================
 * DEMO USAGE
 ====================*/

/*Show some widget. It might be required to increase `LV_MEM_SIZE` */
/*��ʾһЩС������������Ҫ���� `LV_MEM_SIZE` */
#define LV_USE_DEMO_WIDGETS        0
#if LV_USE_DEMO_WIDGETS
#define LV_DEMO_WIDGETS_SLIDESHOW  0
#endif

/*Demonstrate the usage of encoder and keyboard*/
/*��ʾ�������ͼ��̵�ʹ��*/
#define LV_USE_DEMO_KEYPAD_AND_ENCODER     0

/*Benchmark your system*/
/*��ϵͳ���л�׼����*/
#define LV_USE_DEMO_BENCHMARK   0

/*Stress test for LVGL*/
/*LVGL��ѹ������*/
#define LV_USE_DEMO_STRESS      0

/*Music player demo*/
/*���ֲ�������ʾ*/
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

#endif /*End of "Content enable"--�������õĽ���*/
