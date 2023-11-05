#include "battery_box.h"
#include "monitor.h"
#include <stdio.h>
//#include "lv_drv_conf.h"





static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t s_style_common;
static lv_obj_t *slider_1, *slider_2, *slider_3, *slider_4;
static const char *two_line_map[] = {"#ff0000 CURRENT#","\n","#00ff00 VALUE#",""};
static void Battery_Box_In_Ui(lv_obj_t * parent);
static void wave_animation(lv_obj_t * TargetObject, int delay,int end_value);
static void particle_animation(lv_obj_t * TargetObject, int delay_time,int end_value);
static void ofs_set_y_anim(void * img, int32_t v);
static void ofs_set_x_anim(void  * a, int32_t v);
static void Battery_Lable_png(char* title,png_direction direction,lv_obj_t * parent,lv_obj_t * align_parent,lv_coord_t x_ofs,lv_coord_t y_ofs);
static void Battery_Lable(const char * title[],lv_obj_t * parent,lv_obj_t * align_parent,lv_coord_t x_ofs,lv_coord_t y_ofs);
static void slider_1_ofs_y(void * slider, int32_t v);
/******************************************************************************/
void Battery_Box_Ui(lv_obj_t *parent)
{
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,NULL,png_load_path(battery_box.png),png_load_path(battery_box.png));
    /* 设置按钮大小 */
    lv_obj_set_size(Imgbtn_MC,200,120);
    /* 设置按钮位置 */
    lv_obj_align_to(Imgbtn_MC,parent,LV_ALIGN_LEFT_MID,0,-60);
    /*Create a transition animation on width transformation and recolor.*/
    static lv_style_prop_t tr_prop[] = {LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_IMG_RECOLOR_OPA, 0};
    static lv_style_transition_dsc_t tr;
    lv_style_transition_dsc_init(&tr, tr_prop, lv_anim_path_linear, 200, 0, NULL);

    static lv_style_t style_def;
    lv_style_init(&style_def);
    lv_style_set_text_color(&style_def, lv_color_white());
    lv_style_set_transition(&style_def, &tr);
    lv_obj_add_style(Imgbtn_MC,&s_style_common,LV_STATE_PRESSED);

    /*Darken the button when pressed and make it wider*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_img_recolor_opa(&style_pr, LV_OPA_30);
    lv_style_set_img_recolor(&style_pr, lv_color_black());
    lv_style_set_transform_width(&style_pr, 20);

    lv_obj_add_style(Imgbtn_MC,&style_def,0);
    lv_obj_add_style(Imgbtn_MC,&style_pr,LV_STATE_PRESSED);
    /* 设置按钮回调 */
    lv_obj_add_event_cb(Imgbtn_MC,Imgbtn_MC_cb,LV_EVENT_ALL,NULL);
}
static void App_btn_Back_Cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* parent = lv_event_get_user_data(e);
    switch ((uint8_t)code) {
        case LV_EVENT_RELEASED:
            {
                lv_obj_del(parent);
            }
            break;
    }
}
static void Imgbtn_MC_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_RELEASED)
         Battery_Box_In_Ui(App_Common_Init("Battery_Box",App_btn_Back_Cb));
}

static void Battery_Box_In_Ui(lv_obj_t * parent)
{
    lv_obj_t * ui_Image_Particle1;
    lv_obj_t * ui_Image_Particle2;
    lv_obj_t * ui_Image_Particle3;
    lv_obj_t * ui_Image_Battery_BG;
    lv_obj_t * ui_Wave2,* ui_Wave1;
    lv_obj_t * break_led,* tsms_led;
    lv_obj_t * apps_slider1,* apps_slider2;
    lv_obj_t * current_bar,*voltage_bar;
    lv_obj_t * soc_label;


    /* 电池soc */
    ui_Image_Particle1 = lv_img_create(parent);
    lv_img_set_src(ui_Image_Particle1,png_load_path(particle_1.png));
    lv_obj_set_height(ui_Image_Particle1, 240/2);
    lv_obj_set_width(ui_Image_Particle1, LV_SIZE_CONTENT);   /// 1
    //lv_obj_align_to(ui_Image_Particle1,NULL, LV_ALIGN_LEFT_MID,50,20);
    lv_obj_align(ui_Image_Particle1,LV_ALIGN_CENTER,0,0);
    lv_obj_add_flag(ui_Image_Particle1, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image_Particle1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_blend_mode(ui_Image_Particle1, LV_BLEND_MODE_ADDITIVE, LV_PART_MAIN | LV_STATE_DEFAULT);


    ui_Image_Particle2 = lv_img_create(parent);
    lv_img_set_src(ui_Image_Particle2,png_load_path(particle_2.png));
    lv_obj_set_height(ui_Image_Particle2, 240/2);
    lv_obj_set_width(ui_Image_Particle2, LV_SIZE_CONTENT);   /// 1
    lv_obj_align_to(ui_Image_Particle2,ui_Image_Particle1, LV_ALIGN_CENTER,0,0);
    lv_obj_add_flag(ui_Image_Particle2, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image_Particle2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_blend_mode(ui_Image_Particle2, LV_BLEND_MODE_ADDITIVE, LV_PART_MAIN | LV_STATE_DEFAULT);


    ui_Image_Particle3 = lv_img_create(parent);
    lv_img_set_src(ui_Image_Particle3,png_load_path(particle_3.png));
    lv_obj_set_height(ui_Image_Particle3, 240/2);
    lv_obj_set_width(ui_Image_Particle3, LV_SIZE_CONTENT);   /// 1
    lv_obj_align_to(ui_Image_Particle3,ui_Image_Particle1, LV_ALIGN_CENTER,0,0);
    lv_obj_add_flag(ui_Image_Particle3, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image_Particle3, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_blend_mode(ui_Image_Particle3, LV_BLEND_MODE_ADDITIVE, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Wave1 = lv_img_create(parent);
    lv_img_set_src(ui_Wave1,png_load_path(water_2.png));
    lv_obj_set_width(ui_Wave1, 103/2 - 5);
    lv_obj_set_height(ui_Wave1, 130/2 - 5);
    lv_obj_align_to(ui_Wave1,ui_Image_Particle1, LV_ALIGN_CENTER,0,20);
    lv_obj_add_flag(ui_Wave1, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Wave1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_shadow_opa(ui_Wave1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui_Wave1,LV_OPA_80,LV_STATE_DEFAULT);

    ui_Wave2 = lv_img_create(parent);
    lv_img_set_src(ui_Wave2,png_load_path(water_1.png));
    lv_obj_set_width(ui_Wave2,103/2 - 5);
    lv_obj_set_height(ui_Wave2, 130/2 - 5);
    lv_obj_align_to(ui_Wave2,ui_Image_Particle1, LV_ALIGN_CENTER,0,29);
    lv_obj_add_flag(ui_Wave2, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Wave2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_shadow_opa(ui_Wave2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_img_opa(ui_Wave2,LV_OPA_80,LV_STATE_DEFAULT);

    ui_Image_Battery_BG = lv_img_create(parent);
    lv_img_set_src(ui_Image_Battery_BG,png_load_path(battery_bg.png));
    lv_obj_set_width(ui_Image_Battery_BG, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Image_Battery_BG, LV_SIZE_CONTENT);    /// 1
    lv_obj_align_to(ui_Image_Battery_BG,ui_Image_Particle1, LV_ALIGN_CENTER,0,0);
    lv_obj_add_flag(ui_Image_Battery_BG, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(ui_Image_Battery_BG, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    lv_obj_set_style_radius(ui_Image_Battery_BG, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_Image_Battery_BG, lv_color_hex(0xffff00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(ui_Image_Battery_BG, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_Image_Battery_BG, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_Image_Battery_BG, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

    /*Set only the properties that should be different*/
    static lv_style_t style_warning;
    lv_style_init(&style_warning);
    lv_style_set_bg_color(&style_warning, lv_palette_main(LV_PALETTE_YELLOW));
    lv_style_set_border_color(&style_warning, lv_palette_darken(LV_PALETTE_YELLOW, 3));
    lv_style_set_text_color(&style_warning, lv_palette_darken(LV_PALETTE_YELLOW, 4));
    lv_style_set_border_width(&style_warning, 2);
    lv_style_set_radius(&style_warning, 10);
    lv_style_set_shadow_width(&style_warning, 10);
    lv_style_set_shadow_ofs_y(&style_warning, 5);
    lv_style_set_shadow_opa(&style_warning, LV_OPA_50);
    lv_style_set_width(&style_warning, 100);
    lv_style_set_height(&style_warning,LV_SIZE_CONTENT);

    /* 电池SOC标签 */
    soc_label = lv_label_create(parent);
    lv_obj_align_to(soc_label,ui_Image_Battery_BG,LV_ALIGN_OUT_BOTTOM_MID,-35,30);
    lv_label_set_text_fmt(soc_label,"  SOC:%d",9999);
    lv_obj_add_style(soc_label,&style_warning,0);



    /* 制动指示灯 */
    break_led  = lv_led_create(parent);
    lv_obj_align_to(break_led,ui_Image_Particle1, LV_ALIGN_OUT_BOTTOM_RIGHT, 100, -150);
    lv_led_set_brightness(break_led, 150);
    lv_obj_set_height(break_led,50);
    lv_obj_set_width(break_led,50);
    lv_led_set_color(break_led, lv_palette_main(LV_PALETTE_RED));
    lv_led_on(break_led);

    /* 安全回路指示灯 */
    tsms_led  = lv_led_create(parent);
    lv_obj_align_to(tsms_led,ui_Image_Particle1, LV_ALIGN_OUT_BOTTOM_RIGHT, 100, 30);
    lv_led_set_brightness(tsms_led, 150);
    lv_obj_set_height(tsms_led,50);
    lv_obj_set_width(tsms_led,50);
    lv_led_set_color(tsms_led, lv_palette_main(LV_PALETTE_RED));
    lv_led_off(tsms_led);

    /* 油门踏板信号量1 */
    apps_slider1 = lv_slider_create(parent);
    lv_slider_set_range(apps_slider1,0,5000);
    lv_obj_set_style_anim_time(apps_slider1, 5000, LV_STATE_DEFAULT);
    lv_slider_set_value(apps_slider1, 3080, LV_ANIM_ON);
    if(lv_slider_get_mode(apps_slider1) == LV_SLIDER_MODE_RANGE)
        lv_slider_set_left_value(apps_slider1, 0, LV_ANIM_OFF);
    lv_obj_set_width(apps_slider1, 20);
    lv_obj_set_height(apps_slider1, 235);
    lv_obj_align_to(apps_slider1,ui_Image_Particle1, LV_ALIGN_CENTER,250,30);
    lv_obj_set_style_radius(apps_slider1, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(apps_slider1, lv_color_hex(0xD0D7DF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(apps_slider1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(apps_slider1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(apps_slider1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(apps_slider1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(apps_slider1, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(apps_slider1, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(apps_slider1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(apps_slider1, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(apps_slider1, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(apps_slider1, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(apps_slider1, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(apps_slider1, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(apps_slider1, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(apps_slider1, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(apps_slider1, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(apps_slider1, 4, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(apps_slider1, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(apps_slider1, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(apps_slider1, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(apps_slider1, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(apps_slider1, png_load_path(img_slider.png), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(apps_slider1, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    /* 油门踏板信号量2 */
    apps_slider2 = lv_slider_create(parent);
    lv_slider_set_range(apps_slider2,0,5000);
    lv_obj_set_style_anim_time(apps_slider2, 5000, LV_STATE_DEFAULT);
    lv_slider_set_value(apps_slider2, 2080, LV_ANIM_ON);
    if(lv_slider_get_mode(apps_slider2) == LV_SLIDER_MODE_RANGE)
        lv_slider_set_left_value(apps_slider2, 0, LV_ANIM_OFF);
    lv_obj_set_width(apps_slider2, 20);
    lv_obj_set_height(apps_slider2, 235);
    lv_obj_align_to(apps_slider2,ui_Image_Particle1, LV_ALIGN_CENTER,350,30);
    lv_obj_set_style_radius(apps_slider2, 4, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(apps_slider2, lv_color_hex(0xD0D7DF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(apps_slider2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(apps_slider2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(apps_slider2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_color(apps_slider2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_opa(apps_slider2, 255, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_width(apps_slider2, 2, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_outline_pad(apps_slider2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(apps_slider2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_opa(apps_slider2, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(apps_slider2, 20, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(apps_slider2, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(apps_slider2, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(apps_slider2, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(apps_slider2, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(apps_slider2, 1, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_radius(apps_slider2, 4, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(apps_slider2, lv_color_hex(0xFFFFFF), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(apps_slider2, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_main_stop(apps_slider2, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_stop(apps_slider2, 255, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(apps_slider2, png_load_path(img_slider.png), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(apps_slider2, 0, LV_PART_KNOB | LV_STATE_DEFAULT);

    /* 电流信号 */
    current_bar = lv_bar_create(parent);
    lv_obj_set_size(current_bar, 260, 20);
    lv_bar_set_range(current_bar,0,5000);
    lv_obj_align_to(current_bar,ui_Image_Particle1, LV_ALIGN_OUT_BOTTOM_LEFT,-350,-100);
    lv_bar_set_mode(current_bar, LV_BAR_MODE_SYMMETRICAL);
    lv_obj_set_style_anim_time(current_bar, 5000, LV_STATE_DEFAULT);
    lv_bar_set_value(current_bar, 3090, LV_ANIM_ON);
    lv_bar_set_start_value(current_bar, 0, LV_ANIM_ON);
    lv_obj_set_style_bg_img_src(current_bar, png_load_path(img_skew_strip.png), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(current_bar, LV_OPA_30, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_tiled(current_bar,true,LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(current_bar,1,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(current_bar,lv_color_hex(0x00ff80),LV_PART_MAIN);

    /* 电压信号 */
    voltage_bar = lv_bar_create(parent);
    lv_obj_set_size(voltage_bar, 260, 20);
    lv_bar_set_range(voltage_bar,0,5000);
    lv_obj_align_to(voltage_bar,ui_Image_Particle1, LV_ALIGN_OUT_BOTTOM_LEFT,-350,50);
    lv_bar_set_mode(voltage_bar, LV_BAR_MODE_SYMMETRICAL);
    lv_obj_set_style_anim_time(voltage_bar, 5000, LV_STATE_DEFAULT);
    lv_bar_set_value(voltage_bar, 4090, LV_ANIM_ON);
    lv_bar_set_start_value(voltage_bar, 0, LV_ANIM_ON);
    lv_obj_set_style_arc_rounded(voltage_bar,true,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_src(voltage_bar, png_load_path(img_skew_strip.png), LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_opa(voltage_bar, LV_OPA_30, LV_PART_KNOB | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_img_tiled(voltage_bar,true,LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_border_width(voltage_bar,1,LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(voltage_bar,lv_color_hex(0x00ff80),LV_PART_MAIN);

    /* 给每个数据显示添加标签说明 */
    //Battery_Lable_png("CURRENT VLAUE",horzi_direct_png,parent,current_bar,0,0);
    Battery_Lable_png("VOLTAGE VLAUE",horzi_direct_png,parent,voltage_bar,0,0);
    Battery_Lable_png("APPS1",verti_direct_png,parent,apps_slider1,0,15);
    Battery_Lable_png("APPS2",verti_direct_png,parent,apps_slider2,0,15);
    Battery_Lable_png("BREAK",verti_direct_png,parent,break_led,0,15);
    Battery_Lable_png("TSMS",verti_direct_png,parent,tsms_led,0,20);
    Battery_Lable_png(" SOC ",verti_direct_png,parent,ui_Image_Particle1,0,0);

    Battery_Lable(two_line_map,parent,current_bar,0,-10);



    /* 电池soc动画部分 */
    wave_animation(ui_Wave1, 2920,-207);
    wave_animation(ui_Wave2, 4920,-207);
    particle_animation(ui_Image_Particle1,6920,-240);
    particle_animation(ui_Image_Particle2,9920,-240);
    particle_animation(ui_Image_Particle3,2290,-240);


}
/********************************************************************************************/
/* 这里的图片显示数据使用一个btnmatrix部件
 * 1.使用两个btn进行上下或者左右排列，虚线边框，明显颜色
 * 2.使用arc部件在下面用一个半圆作为承接
 * 3.最后使用bar部件，多画几条线进行指示
 * 4.同时其他的父对象对齐与位置偏移参数保留
*/
static void Battery_Lable(const char *title[],lv_obj_t * parent,lv_obj_t * align_parent,lv_coord_t x_ofs,lv_coord_t y_ofs)
{
    lv_obj_t *btn_matrix2, *arc_left, *arc_right;

    /* 创建按键矩阵，2个按键 */
    btn_matrix2 = lv_btnmatrix_create(parent);
    /* 配置按键矩阵的大小 */
    lv_obj_set_size(btn_matrix2, 150, 100);
    /* 配置按键矩阵的颜色与背景色一致 */
    lv_obj_set_style_bg_color(btn_matrix2,lv_obj_get_style_bg_color(parent,LV_PART_MAIN),LV_PART_MAIN);
    /* 配置按键矩阵的边框颜色与背景色一致 */
    lv_obj_set_style_border_color(btn_matrix2,lv_obj_get_style_bg_color(parent,LV_PART_MAIN),LV_PART_MAIN);
    /* 配置按键矩阵的相对位置 */
    lv_obj_align_to(btn_matrix2,align_parent,LV_ALIGN_OUT_TOP_MID,x_ofs,y_ofs);
    /* 配置按键矩阵，给两个按键配置名字 */
    lv_btnmatrix_set_map(btn_matrix2,title);
    /* 配置按键矩阵，两个按键的宽度 */
    lv_btnmatrix_set_btn_width(btn_matrix2,0,1);
    lv_btnmatrix_set_btn_width(btn_matrix2,1,1);
    /* 配置按键类型为禁用 */
    lv_btnmatrix_set_btn_ctrl(btn_matrix2, 0, LV_BTNMATRIX_CTRL_CHECKED | LV_BTNMATRIX_CTRL_RECOLOR);
    lv_btnmatrix_set_btn_ctrl(btn_matrix2, 1, LV_BTNMATRIX_CTRL_CHECKED | LV_BTNMATRIX_CTRL_RECOLOR);


    /* 创建左边圆弧部件 */
    arc_left = lv_arc_create(parent);
    /* 配置背景弧的起始角度和终止角度 */
    lv_arc_set_bg_angles(arc_left,135,250);
    /* 配置前景弧的起始角度和终止角度 */
    lv_arc_set_angles(arc_left,135,250);
    /* 配置圆弧部件的大小 */
    lv_obj_set_size(arc_left,50,50);
    /* 将圆弧部件放在左下角 */
    lv_obj_align_to(arc_left,btn_matrix2,LV_ALIGN_OUT_BOTTOM_LEFT,0,-40);
    /* 去除旋钮 */
    lv_obj_remove_style(arc_left,NULL,LV_PART_KNOB);
    /* 配置圆弧指示色为橙色 */
    lv_obj_set_style_arc_color(arc_left,lv_palette_main(LV_PALETTE_ORANGE),LV_PART_INDICATOR);
    /* 配置圆弧背景色为紫色 */
    lv_obj_set_style_arc_color(arc_left,lv_palette_main(LV_PALETTE_PURPLE),LV_PART_MAIN);
    /* 配置圆弧一半的进度 */
    lv_arc_set_value(arc_left,50);
    /* 配置圆弧指示器的宽度 */
    lv_obj_set_style_arc_width(arc_left,8,LV_PART_INDICATOR);
    /* 配置圆弧背景的宽度 */
    lv_obj_set_style_arc_width(arc_left,7,LV_PART_MAIN);

    /* 创建右边圆弧部件 */
    arc_right = lv_arc_create(parent);
    /* 配置背景弧的起始角度和终止角度 */
    lv_arc_set_bg_angles(arc_right,280,45);
    /* 配置前景弧的起始角度和终止角度 */
    lv_arc_set_angles(arc_right,280,45);
    /* 配置圆弧部件的大小 */
    lv_obj_set_size(arc_right,50,50);
    /* 将圆弧部件放在左下角 */
    lv_obj_align_to(arc_right,btn_matrix2,LV_ALIGN_OUT_BOTTOM_RIGHT,0,-40);
    /* 去除旋钮 */
    lv_obj_remove_style(arc_right,NULL,LV_PART_KNOB);
    /* 配置圆弧背景色为橙色 */
    lv_obj_set_style_arc_color(arc_right,lv_palette_main(LV_PALETTE_BROWN),LV_PART_MAIN);
    /* 配置圆弧指示色为紫色 */
    lv_obj_set_style_arc_color(arc_right,lv_palette_main(LV_PALETTE_LIME),LV_PART_INDICATOR);
    /* 配置圆弧一半的进度 */
    lv_arc_set_value(arc_right,50);
    /* 配置圆弧指示器的宽度 */
    lv_obj_set_style_arc_width(arc_right,8,LV_PART_INDICATOR);
    /* 配置圆弧背景的宽度 */
    lv_obj_set_style_arc_width(arc_right,7,LV_PART_MAIN);

    /* 创建滑块部件1来进行动态展示 */
    slider_1 = lv_slider_create(parent);
    /* 放在按键矩阵的下面 */
    lv_obj_align_to(slider_1,btn_matrix2,LV_ALIGN_OUT_BOTTOM_MID,150,-25);
    /* 配置滑块部件1的大小 */
    lv_obj_set_size(slider_1,10,40);
    /* 去除旋钮 */
    lv_obj_remove_style(slider_1,NULL,LV_PART_KNOB);
    /* 配置指示器颜色为黄色 */
    lv_obj_set_style_bg_color(slider_1,lv_palette_main(LV_PALETTE_LIME),LV_PART_INDICATOR);
    /* 配置背景色与parent背景色一致 */
    lv_obj_set_style_bg_color(slider_1,lv_obj_get_style_bg_color(parent,LV_PART_MAIN),LV_PART_MAIN);
    /* 配置滑块部件的数值范围 */
    lv_slider_set_range(slider_1,0,100);

    //lv_obj_set_style_transform_angle(slider_1,450,0);

    /* 将按钮矩阵置顶 */
    //lv_obj_move_foreground(btn_matrix2);


    /* 配置滑块部件1的动画属性 */
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a,slider_1);
    lv_anim_set_time(&a,500);
    lv_anim_set_values(&a,0,100);
    lv_anim_set_repeat_count(&a,LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_playback_time(&a,500);
    lv_anim_set_exec_cb(&a,(lv_anim_exec_xcb_t)slider_1_ofs_y);
    lv_anim_start(&a);
}

static void Battery_Lable_png(char * title,png_direction direction,lv_obj_t * parent,lv_obj_t * align_parent,lv_coord_t x_ofs,lv_coord_t y_ofs)
{
    lv_obj_t * png_direction;
    lv_obj_t * png_label;


    png_direction = lv_img_create(parent);
    if(direction == horzi_direct_png){
        lv_img_set_src(png_direction,png_load_path(horzi_direction.png));
    }
    else {
        lv_img_set_src(png_direction,png_load_path(verti_direction.png));
    }
    lv_obj_set_height(png_direction, LV_SIZE_CONTENT);
    lv_obj_set_width(png_direction, LV_SIZE_CONTENT);   /// 1
    lv_obj_align_to(png_direction,align_parent,LV_ALIGN_OUT_TOP_MID,x_ofs,y_ofs);
    lv_obj_add_flag(png_direction, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    lv_obj_clear_flag(png_direction, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    png_label = lv_label_create(png_direction);
    lv_label_set_text(png_label,title);
    if(direction == horzi_direct_png){
        lv_obj_align_to(png_label,png_direction,LV_ALIGN_CENTER,0,-15);
    }
    else {
        lv_obj_align_to(png_label,png_direction,LV_ALIGN_CENTER,0,0);
    }

    lv_obj_set_style_text_color(png_label,lv_color_white(), LV_STATE_DEFAULT);
}
static void particle_animation(lv_obj_t * TargetObject, int delay_time,int end_value)
{
    lv_anim_t c;
    lv_anim_init(&c);
    lv_anim_set_exec_cb(&c, ofs_set_y_anim);
    lv_anim_set_var(&c, TargetObject);
    lv_anim_set_time(&c, delay_time);
    lv_anim_set_values(&c, 0, end_value);
    lv_anim_set_path_cb(&c, lv_anim_path_linear);
    lv_anim_set_delay(&c,0);
    lv_anim_set_playback_time(&c, 0);
    lv_anim_set_playback_delay(&c,0);
    lv_anim_set_repeat_count(&c, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&c, 0);
    lv_anim_set_early_apply(&c, false);
    lv_anim_start(&c);
}
static void wave_animation(lv_obj_t * TargetObject, int delay_time,int end_value)
{
    lv_anim_t c;
    lv_anim_init(&c);
    lv_anim_set_var(&c, TargetObject);
    lv_anim_set_time(&c, delay_time);
    lv_anim_set_exec_cb(&c, ofs_set_x_anim);
    lv_anim_set_values(&c,0, end_value);
    lv_anim_set_path_cb(&c, lv_anim_path_linear);
    lv_anim_set_delay(&c,0);
    lv_anim_set_playback_time(&c, 0);
    lv_anim_set_playback_delay(&c,0);
    lv_anim_set_repeat_count(&c, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&c, 0);
    lv_anim_set_early_apply(&c, false);
    lv_anim_start(&c);
}

static void ofs_set_y_anim(void * img, int32_t v)
{
    lv_img_set_offset_y(img, v);
    //printf("ofs y = %d\n",v);
}
static void ofs_set_x_anim(void  * a, int32_t v)
{
    lv_img_set_offset_x(a, v);
    //printf("ofs x = %d\n",v);
}
static void slider_1_ofs_y(void * slider, int32_t v)
{
    lv_slider_set_value(slider, 100 - v,LV_ANIM_ON);
    //printf("slider ofs y = %d\n",v);
}
/*******************************************************************************************/
