#include "monitor.h"
#include "lvgl/lvgl.h"
#include <stdio.h>
#include "motor_control_ui.h"


static lv_obj_t* Monitor_Speed_Meter;
//static lv_obj_t *image;
static void lv_example_Monitor_Speed_Meter(void);

#define scr_act_height() lv_obj_get_height(lv_scr_act())
#define scr_act_width()  lv_obj_get_width(lv_scr_act())

static void Monitor_Main_Style(lv_obj_t* Monitor_Speed_Meter);
static void Monitor_Main_label(lv_coord_t weight, lv_coord_t height);
/* 每个按钮打开的页面公用BACK回退按钮 */
static void Monitor_Back_cb(lv_obj_t* obj,lv_event_t event);
/* 每个按钮打开的页面公用的初始化函数 */
static lv_obj_t* Monitor_Common_Init(lv_obj_t*parent,const char *title);

//LV_IMG_DECLARE(motor_control)

void Gui_Monitor_App()
{

    uint16_t meter_height = 0;
    meter_height = ((unsigned short)(scr_act_height() * 0.8));

    /* 创建速度盘 */
    lv_example_Monitor_Speed_Meter();

//    /* 使用C数组加载图片 */
//    image = lv_img_create(lv_scr_act());
//    lv_img_set_src(image,&motor_control);
//    lv_obj_align(image,LV_ALIGN_TOP_RIGHT,0,0);
//    printf("gitee test\n");
    Motor_Control_UI(lv_scr_act());
}
void Monitor_Main_Style(lv_obj_t * Monitor_Speed_Meter)
{
    lv_style_t style_speed_meter;
    //创建普通样式
    //样式初始化
    lv_style_init(&style_speed_meter);
    //设置背景--黑色
    lv_style_set_bg_color(&style_speed_meter, lv_color_hex(0x0000ff));
    //设置背景透明度--20
    lv_style_set_bg_opa(&style_speed_meter, 80);
    //设置字体颜色--黄色
    lv_style_set_text_color(&style_speed_meter, lv_color_hex(0xffff00));
    //设置速度盘背景渐变颜色方向--垂直向下
    lv_style_set_bg_grad_dir(&style_speed_meter, LV_GRAD_DIR_VER);
    //设置背景梯度起点属性--渐变从0顶部开始
    lv_style_set_bg_grad_stop(&style_speed_meter, 255);
    //设置背景梯度颜色属性
    lv_style_set_bg_grad_color(&style_speed_meter, lv_palette_main(LV_PALETTE_GREEN));
    //设置边框颜色--蓝色
    lv_style_set_border_color(&style_speed_meter, lv_palette_main(LV_PALETTE_BLUE));
    //设置边框的宽度--3
    lv_style_set_border_width(&style_speed_meter, 3);
    //设置速度盘的轮廓属性--3
    lv_style_set_outline_width(&style_speed_meter, 3);
    //设置轮廓属性的颜色--橙色
    lv_style_set_outline_color(&style_speed_meter, lv_palette_main(LV_PALETTE_ORANGE));
    //设置轮廓间隙属性--3
    lv_style_set_outline_pad(&style_speed_meter, 3);

    //设置速度盘对象的样式
    lv_obj_add_style(Monitor_Speed_Meter, &style_speed_meter, LV_PART_MAIN);
}
void Monitor_Main_label(lv_coord_t weight,lv_coord_t height)
{
    lv_obj_t* label_speed_value;

    //设置速度值文本标签
    label_speed_value = lv_label_create(Monitor_Speed_Meter);
    //设置文本与颜色
    lv_label_set_text(label_speed_value, "Km/h");
    //设置文本对齐方式
    lv_obj_set_pos((lv_obj_t*)(label_speed_value),(short)(weight * 0.45), (short)(height * 0.7));
}
/**
 * @brief 仪表2的回调函数
 * @param indic 不知道是啥？
 * @param value 数据
*/
static void anim_2_indic_cb(lv_meter_indicator_t* indic, int32_t value)
{

    lv_meter_set_indicator_end_value(Monitor_Speed_Meter, indic, value);
}
static void lv_example_Monitor_Speed_Meter(void)
{
    const lv_font_t* font;

    /* 根据屏幕大小设置字体 */
    if (scr_act_width() <= 320)
        font = &lv_font_montserrat_8;
    else if (scr_act_width() < 480)
        font = &lv_font_montserrat_10;
    else
        font = &lv_font_montserrat_24;


    /* 定义并创建仪表 */
    Monitor_Speed_Meter = lv_meter_create(lv_scr_act());
    /* 设置仪表宽度 */
    lv_obj_set_width(Monitor_Speed_Meter, (short)(scr_act_height() * 0.8));
    /* 设置仪表高度 */
    lv_obj_set_height(Monitor_Speed_Meter, (short)(scr_act_height() * 0.8));
    /* 设置仪表位置 */
    lv_obj_center(Monitor_Speed_Meter);
    /* 设置仪表文字字体 */
    lv_obj_set_style_text_font(Monitor_Speed_Meter, font, LV_PART_MAIN);
    /* 删除仪表指针样式 */
    //lv_obj_remove_style(Monitor_Speed_Meter, NULL, LV_PART_INDICATOR);

    /* 设置仪表刻度 */
    /* 添加刻度 */
    lv_meter_scale_t* scale = lv_meter_add_scale(Monitor_Speed_Meter);
    /* 设置小刻度 */
    lv_meter_set_scale_ticks(Monitor_Speed_Meter,
                            scale,
                            41,//小刻度的数量
                            2,//小刻度的宽度
                            (unsigned short)(scr_act_height() / 15),//小刻度的长度
                            lv_palette_main(LV_PALETTE_YELLOW)//小刻度的颜色
                            );
    /* 设置主刻度 */
    lv_meter_set_scale_major_ticks(Monitor_Speed_Meter,
                                    scale,
                                    8,//绘画主刻度的步长
                                    4,//主刻度的宽度
                                    (unsigned short)(scr_act_height() / 13),//主刻度的长度
                                    lv_color_hex3(0xEEE),//主刻度的颜色
                                    scr_act_height() / 40//刻度与标签之间的间隙
                                    );
    /* 设置仪表的角度和仪表的范围 */
    lv_meter_set_scale_range(Monitor_Speed_Meter, scale, 0, 100, 270, 135);
    /* 添加指针 */
    lv_meter_indicator_t * indic = lv_meter_add_needle_line(Monitor_Speed_Meter,
                             scale,
                             3,
                             lv_color_hex3(0xf34),
                             -6
                            );
    /* 设置指针指向的数值 */
    lv_meter_set_indicator_value(Monitor_Speed_Meter,
        indic,
        33
    );
    /* 设置指针 */
    lv_meter_indicator_t* indic1 = lv_meter_add_arc(Monitor_Speed_Meter,
                                                    scale,
                                                    (unsigned short)(scr_act_height() / 45),
                                                    lv_palette_main(LV_PALETTE_RED),//红
                                                    0
                                                    );
    lv_meter_indicator_t* indic2 = lv_meter_add_arc(Monitor_Speed_Meter,
                                                    scale,
                                                    (unsigned short)(scr_act_height() / 45),
                                                    lv_palette_main(LV_PALETTE_GREEN),//绿
                                                    -scr_act_height() / 45
                                                    );
    lv_meter_indicator_t* indic3 = lv_meter_add_arc(Monitor_Speed_Meter,
                                                    scale,
                                                    (unsigned short)(scr_act_height() / 45),
                                                    lv_palette_main(LV_PALETTE_BLUE),//蓝
                                                    -(short)(scr_act_height() / 22.5)
                                                    );





    /* 设置指针动画 */
    lv_anim_t a;
    /* 初始化动画对象 */
    lv_anim_init(&a);
    /* 设置动画函数 */
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)anim_2_indic_cb);
    /* 设置动画的起始值和结束值 */
    lv_anim_set_values(&a, 0, 100);
    /* 重复前延迟，默认为0(禁用)[ms] */
    lv_anim_set_repeat_delay(&a, 100);
    /* 延迟后在回放，默认为0(禁用)[ms] */
    lv_anim_set_playback_delay(&a, 100);
    /* 重复的数量，默认数值为1，LV_ANIM_REPEAT_INFINITE用于无限重复 */
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    /* 设置动画时间长度 */
    lv_anim_set_time(&a, 2000);
    /* 准备好后，按此持续时间倒放动画，默认为0(禁用)[ms] */
    lv_anim_set_playback_time(&a, 500);
    /* 设置动画的对象 */
    lv_anim_set_var(&a, indic1);
    /* 开始动画 */
    lv_anim_start(&a);


    /* 设置动画的时间长度 */
    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_set_var(&a, indic2);
    lv_anim_start(&a);


    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_var(&a, indic3);
    lv_anim_start(&a);

    //创建样式
    Monitor_Main_Style(Monitor_Speed_Meter);
    //创建速度值标签
    Monitor_Main_label((short)(scr_act_height() * 0.8),(short)(scr_act_height() * 0.8));
//    lv_obj_t * img = lv_img_create(lv_scr_act());
//    //lv_obj_center(img);
//    lv_obj_set_pos(img,60,300);
//    lv_img_set_src(img,"E:/Ls_Monitor/LVGL_Monitor/images/pack.png");
}

static void Monitor_Back_cb(lv_obj_t* obj,lv_event_t event)
{

}
static lv_obj_t* Monitor_Common_Init(lv_obj_t*parent,const char *title)
{
    static lv_style_t Btn_Back_Style;
    static lv_style_t UI_Bg_Style;


}


