#include "cool_control.h"
#include "monitor.h"
#include <stdio.h>
//#include "lv_drv_conf.h"


static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t s_style_common;
static void Cool_Control_In_Ui(lv_obj_t* parent);
static void png_scroll_anim(lv_obj_t* parent);
static void lv_fanpng_set_scroll(void *, int32_t);
static void knob_scroll(lv_event_t * e);
static void draw_event_cb(lv_event_t * e);



void Cool_Control_Ui(lv_obj_t *parent)
{
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,NULL,png_load_path(cool_control.png),png_load_path(cool_control.png));
    /* 设置按钮大小 */
    lv_obj_set_size(Imgbtn_MC,200,120);
    /* 设置按钮位置 */
    lv_obj_align_to(Imgbtn_MC,parent,LV_ALIGN_RIGHT_MID,0,60);
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
         Cool_Control_In_Ui(App_Common_Init("Cool_Control",App_btn_Back_Cb));
}

void Cool_Control_In_Ui(lv_obj_t* parent)
{
    /* 创建风扇图片 */
    lv_obj_t * fan_png;
    fan_png = lv_img_create(parent);
    lv_img_set_src(fan_png,png_load_path(fan.png));
    lv_obj_align(fan_png,LV_ALIGN_TOP_LEFT,30,30);
    lv_obj_set_style_img_recolor(fan_png,lv_color_hex(0xf2f2f2),LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(fan_png,128,LV_PART_MAIN);
    /* 配置图片旋转中心点 */
    lv_img_set_pivot(fan_png,
                     100,//lv_obj_get_x(fan_png) + 100,
                     100//lv_obj_get_y(fan_png) + 100
                     );
    /* 创建水泵图片 */
    lv_obj_t * pump_png;
    pump_png = lv_img_create(parent);
    lv_img_set_src(pump_png,png_load_path(pump.png));
    lv_obj_align(pump_png,LV_ALIGN_TOP_RIGHT,-30,30);
    lv_obj_set_style_img_recolor(pump_png,lv_color_hex(0xf2f2f2),LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(pump_png,128,LV_PART_MAIN);

    /* 创建旋钮图片 */
    lv_obj_t * knob;
    knob = lv_img_create(parent);
    lv_img_set_src(knob,png_load_path(knob.png));
    lv_obj_align(knob,LV_ALIGN_TOP_MID,0,30);
    lv_obj_set_style_img_recolor(knob,lv_color_hex(0xf2f2f2),LV_PART_MAIN);
    lv_obj_set_style_img_recolor_opa(knob,128,LV_PART_MAIN);
    lv_obj_t * knob_arc;
    knob_arc = lv_arc_create(parent);
    lv_arc_set_range(knob_arc,0,100);
    lv_obj_set_size(knob_arc,190,190);
    lv_arc_set_start_angle(knob_arc,180);
    lv_arc_set_end_angle(knob_arc,90);
    lv_arc_set_rotation(knob_arc,45);
    lv_arc_set_value(knob_arc,0);
    lv_obj_align_to(knob_arc,knob,LV_ALIGN_CENTER,0,0);
    lv_obj_add_event_cb(knob_arc,knob_scroll,LV_EVENT_VALUE_CHANGED,knob);
    /* knob_arc style */
    lv_obj_set_style_arc_color(knob_arc,lv_color_hex(0xfedc00),LV_PART_INDICATOR);
    lv_obj_set_style_arc_opa(knob_arc,128,LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(knob_arc,8,LV_PART_MAIN);
    lv_obj_set_style_arc_width(knob_arc,8,LV_PART_INDICATOR);
    lv_obj_set_style_border_width(knob_arc,2,LV_PART_INDICATOR);
    lv_obj_set_style_border_color(knob_arc,lv_color_hex(0x1e1e27),LV_PART_MAIN);
    /* 风扇旋转动画 */
    png_scroll_anim(fan_png);

    /* 电机控制器的四个电机的温度图表 */
    lv_obj_t *temp_chart;
    temp_chart = lv_chart_create(parent);
    lv_obj_set_size(temp_chart,210,150);
    lv_obj_align_to(temp_chart,knob,LV_ALIGN_OUT_BOTTOM_MID,0,0);
    lv_chart_set_type(temp_chart,LV_CHART_TYPE_BAR);
    lv_chart_set_range(temp_chart,LV_CHART_AXIS_PRIMARY_Y,0,150);
    lv_chart_set_update_mode(temp_chart,LV_CHART_UPDATE_MODE_SHIFT);
    lv_chart_series_t* ser1 = lv_chart_add_series(temp_chart,lv_color_hex(0x444444),LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_series_t* ser2 = lv_chart_add_series(temp_chart,lv_color_hex(0x123456),LV_CHART_AXIS_PRIMARY_Y);
    lv_chart_set_point_count(temp_chart,4);
    lv_obj_add_event_cb(temp_chart,draw_event_cb,LV_EVENT_DRAW_PART_BEGIN,NULL);
//    lv_chart_set_ext_y_array(temp_chart,ser1,(lv_coord_t*)example_data);
//    lv_chart_set_ext_y_array(temp_chart,ser2,(lv_coord_t*)example_data);
//    lv_chart_set_axis_tick(temp_chart,LV_CHART_AXIS_PRIMARY_X,10,5,12,3,true,50);
    lv_chart_set_axis_tick(temp_chart,LV_CHART_AXIS_PRIMARY_Y,10,5,6,2,true,50);
//    lv_chart_set_axis_tick(temp_chart,LV_CHART_AXIS_SECONDARY_Y,10,5,3,4,true,50);
//    lv_chart_set_zoom_x(temp_chart,80);
    lv_chart_set_next_value(temp_chart,ser1,31);
    lv_chart_set_next_value(temp_chart,ser1,61);
    lv_chart_set_next_value(temp_chart,ser1,91);
    lv_chart_set_next_value(temp_chart,ser1,131);
    lv_chart_set_next_value(temp_chart,ser2,21);
    lv_chart_set_next_value(temp_chart,ser2,51);
    lv_chart_set_next_value(temp_chart,ser2,81);
    lv_chart_set_next_value(temp_chart,ser2,121);
    /* 更新图表 */
    lv_chart_refresh(temp_chart);
}
static void draw_event_cb(lv_event_t * e)
{
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(!lv_obj_draw_part_check_type(dsc, &lv_chart_class, LV_CHART_DRAW_PART_TICK_LABEL)) return;

    if(dsc->id == LV_CHART_AXIS_PRIMARY_X && dsc->text) {
        const char * month[] = {"Temp1","Temp2","Temp3","Temp4"};
        lv_snprintf(dsc->text, dsc->text_length, "%s", month[dsc->value]);
    }
}
static void knob_scroll(lv_event_t * e)
{
    lv_obj_t* _obj_knob = lv_event_get_user_data(e);
    lv_obj_t* _obj_arc = lv_event_get_target(e);
    int16_t value = lv_arc_get_value(_obj_arc);
    lv_img_set_angle(_obj_knob,(int16_t)(value * 27));
}
static void lv_fanpng_set_scroll(void *obj, int32_t value)
{
    /* 参数obj是控件img */
    lv_obj_t * _obj = (lv_obj_t*)obj;
    lv_img_set_angle(_obj,(int16_t)value);
}
static void png_scroll_anim(lv_obj_t* obj)
{
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a,(lv_anim_exec_xcb_t)lv_fanpng_set_scroll);
    lv_anim_set_var(&a,obj);
    lv_anim_set_time(&a,2500);
    lv_anim_set_values(&a,0,3600);
    lv_anim_set_repeat_count(&a,LV_ANIM_REPEAT_INFINITE);
    lv_anim_start((&a));
}
