#include "Monitor.h"
#include "lvgl/lvgl.h"

static lv_obj_t* meter_2;
static void lv_mainstart(void);
#define scr_act_height() lv_obj_get_height(lv_scr_act())
#define scr_act_width()  lv_obj_get_width(lv_scr_act())


void Gui_Monitor_App()
{
    lv_mainstart();
}



/**
 * @brief 仪表1的回调函数
 * @param meter 仪表控件指针
 * @param value 数据
*/
static void anim_1_indic_cb(lv_obj_t* meter, int32_t value)
{
    lv_meter_indicator_t* indic = (lv_meter_indicator_t*)meter->user_data;
    lv_meter_set_indicator_value(meter, indic, value);
}
/**
 * @brief 仪表2的回调函数
 * @param indic 不知道是啥？
 * @param value 数据
*/
static void anim_2_indic_cb(lv_meter_indicator_t* indic, int32_t value)
{
    lv_meter_set_indicator_end_value(meter_2, indic, value);
}
static void lv_example_meter_2(void)
{
    const lv_font_t* font;
    /* 根据屏幕大小设置字体 */
    if (scr_act_width() <= 320)
        font = &lv_font_montserrat_8;
    else if (scr_act_width() <= 480)
        font = &lv_font_montserrat_10;
    else
        font = &lv_font_montserrat_14;
    /* 定义并创建仪表 */
    meter_2 = lv_meter_create(lv_scr_act());
    /* 设置仪表宽度 */
    lv_obj_set_width(meter_2, scr_act_height() * 0.6);
    /* 设置仪表高度 */
    lv_obj_set_height(meter_2, scr_act_height() * 0.6);
    /* 设置仪表位置 */
    lv_obj_center(meter_2);
    /* 设置仪表文字字体 */
    lv_obj_set_style_text_font(meter_2, font, LV_PART_MAIN);
    /* 删除仪表指针样式 */
    lv_obj_remove_style(meter_2, NULL, LV_PART_INDICATOR);
    /* 设置仪表刻度 */
    /* 添加刻度 */
    lv_meter_scale_t* scale = lv_meter_add_scale(meter_2);
    /* 设置小刻度 */
    lv_meter_set_scale_ticks(meter_2,
                            scale,
                            41,//小刻度的数量
                            2,//小刻度的宽度
                            scr_act_height() / 15,//小刻度的长度
                            lv_palette_main(LV_PALETTE_YELLOW)//小刻度的颜色
                            );
    /* 设置主刻度 */
    lv_meter_set_scale_major_ticks(meter_2,
                                    scale,
                                    8,//绘画主刻度的步长
                                    4,//主刻度的宽度
                                    scr_act_height() / 13,//主刻度的长度
                                    lv_color_hex3(0xEEE),//主刻度的颜色
                                    scr_act_height() / 40//刻度与标签之间的间隙
                                    );
    /* 设置仪表的角度和仪表的范围 */
    lv_meter_set_scale_range(meter_2, scale, 0, 100, 270, 135);
    /* 添加指针 */
    lv_meter_indicator_t * indic = lv_meter_add_needle_line(meter_2,
                             scale,
                             3,
                             lv_color_hex3(0xf34),
                             -6
                            );
    /* 设置指针指向的数值 */
    lv_meter_set_indicator_value(meter_2,
        indic,
        33
    );
    /* 设置指针 */
    lv_meter_indicator_t* indic1 = lv_meter_add_arc(meter_2,
                                                    scale,
                                                    scr_act_height() / 45,
                                                    lv_palette_main(LV_PALETTE_RED),//红
                                                    0
                                                    );
    lv_meter_indicator_t* indic2 = lv_meter_add_arc(meter_2,
                                                    scale,
                                                    scr_act_height() / 45,
                                                    lv_palette_main(LV_PALETTE_GREEN),//绿
                                                    -scr_act_height() / 45
                                                    );
    lv_meter_indicator_t* indic3 = lv_meter_add_arc(meter_2,
                                                    scale,
                                                    scr_act_height() / 45,
                                                    lv_palette_main(LV_PALETTE_BLUE),//蓝
                                                    -scr_act_height() / 22.5
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

}
static void lv_example_meter_1(void)
{
    const lv_font_t* font;
    /* 根据屏幕大小设置字体 */
    if (scr_act_width() <= 320)
        font = &lv_font_montserrat_10;
    else if (scr_act_width() <= 480)
        font = &lv_font_montserrat_14;
    else
        font = &lv_font_montserrat_16;
    /* 定义并创建仪表 */
    lv_obj_t* meter = lv_meter_create(lv_scr_act());
    /* 设置仪表宽度 */
    lv_obj_set_width(meter, scr_act_height() * 0.4);
    /* 设置仪表高度 */
    lv_obj_set_height(meter, scr_act_height() * 0.4);
    /* 设置仪表位置 */
    lv_obj_align(meter, LV_ALIGN_CENTER, -scr_act_width() / 3, 0);
    /* 设置仪表文字字体 */
    lv_obj_set_style_text_font(meter, font, LV_PART_MAIN);
    /* 设置仪表刻度 */
    /* 定义并添加刻度 */
    lv_meter_scale_t* scale = lv_meter_add_scale(meter);
    /* 设置小刻度 */
    lv_meter_set_scale_ticks(meter, scale, 41, 1,
                             scr_act_height() / 80,
                             lv_palette_main(LV_PALETTE_GREY));

    /* 设置主刻度 */
    lv_meter_set_scale_major_ticks(meter, scale, 8, 1,
                                    scr_act_height() / 60, lv_color_black(),
                                    scr_act_height() / 30);
    /* 设置指针 */
    lv_meter_indicator_t* indic;
    indic = lv_meter_add_arc(meter, scale, 2,
    lv_palette_main(LV_PALETTE_BLUE), 0);
    lv_meter_set_indicator_start_value(meter, indic, 0);
    lv_meter_set_indicator_end_value(meter, indic, 20);
    indic = lv_meter_add_scale_lines(meter, scale,
    lv_palette_main(LV_PALETTE_BLUE),
    lv_palette_main(LV_PALETTE_BLUE), false, 0);
    lv_meter_set_indicator_start_value(meter, indic, 0);
    lv_meter_set_indicator_end_value(meter, indic, 20);
    indic = lv_meter_add_arc(meter, scale, 2,
    lv_palette_main(LV_PALETTE_RED), 0);
    lv_meter_set_indicator_start_value(meter, indic, 80);
    lv_meter_set_indicator_end_value(meter, indic, 100);
    indic = lv_meter_add_scale_lines(meter, scale,
    lv_palette_main(LV_PALETTE_RED),
    lv_palette_main(LV_PALETTE_RED), false, 0);
    lv_meter_set_indicator_start_value(meter, indic, 80);
    lv_meter_set_indicator_end_value(meter, indic, 100);
    indic = lv_meter_add_needle_line(meter, scale, 4,
    lv_palette_main(LV_PALETTE_GREY), -10);
    lv_obj_set_user_data(meter, indic);
    /* 设置指针动画 */
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)anim_1_indic_cb);
    lv_anim_set_var(&a, meter);
    lv_anim_set_values(&a, 0, 100);
    lv_anim_set_time(&a, 2000);
    lv_anim_set_repeat_delay(&a, 100);
    lv_anim_set_playback_time(&a, 500);
    lv_anim_set_playback_delay(&a, 100);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);
}
void lv_mainstart(void)
{
    lv_example_meter_1();
    lv_example_meter_2();
    
}
