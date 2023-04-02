#include "motor_control_ui.h"
#include "monitor.h"
#include <stdio.h>
//#include "lv_drv_conf.h"


static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t s_style_common;
static void Motor_Control_In_Ui(lv_obj_t *parent);
static void wave_animation(lv_obj_t * TargetObject, uint32_t delay,int32_t end_value);
static void ofs_set_x_anim(void  * a, lv_coord_t v);


void Motor_Control_Ui(lv_obj_t *parent)
{
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,NULL,png_load_path(motor_control.png),png_load_path(motor_control.png));
    /* 设置按钮大小 */
    lv_obj_set_size(Imgbtn_MC,400,120);
    /* 设置按钮位置 */
    lv_obj_align_to(Imgbtn_MC,parent,LV_ALIGN_TOP_LEFT,0,0);
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
        Motor_Control_In_Ui(App_Common_Init("Motor_Control",App_btn_Back_Cb));
}

static void Motor_Control_In_Ui(lv_obj_t *parent)
{
    lv_obj_t * chassis,* wheel_1;
    lv_obj_t * obj_warning1,* obj_warning2,* obj_warning3,* obj_warning4;
    lv_obj_t * label1,*label2,*label3,*label4;

    /*A base style*/
    static lv_style_t style_base;
    lv_style_init(&style_base);
    lv_style_set_bg_color(&style_base, lv_palette_main(LV_PALETTE_LIGHT_BLUE));
    lv_style_set_border_color(&style_base, lv_palette_darken(LV_PALETTE_LIGHT_BLUE, 3));
    lv_style_set_border_width(&style_base, 2);
    lv_style_set_radius(&style_base, 10);
    lv_style_set_shadow_width(&style_base, 10);
    lv_style_set_shadow_ofs_y(&style_base, 5);
    lv_style_set_shadow_opa(&style_base, LV_OPA_50);
    lv_style_set_text_color(&style_base, lv_color_white());
    lv_style_set_width(&style_base, 120);
    lv_style_set_height(&style_base, LV_SIZE_CONTENT);

    /*Set only the properties that should be different*/
    static lv_style_t style_warning;
    lv_style_init(&style_warning);
    lv_style_set_bg_color(&style_warning, lv_palette_main(LV_PALETTE_YELLOW));
    lv_style_set_border_color(&style_warning, lv_palette_darken(LV_PALETTE_YELLOW, 3));
    lv_style_set_text_color(&style_warning, lv_palette_darken(LV_PALETTE_YELLOW, 4));


    /* 电池soc */
    chassis = lv_img_create(parent);
    lv_img_set_src(chassis,png_load_path(chassis.png));
    lv_obj_set_height(chassis, LV_SIZE_CONTENT);
    lv_obj_set_width(chassis, LV_SIZE_CONTENT);   /// 1
    lv_obj_align(chassis,LV_ALIGN_CENTER,0,0);
    lv_obj_add_flag(chassis, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags

    wheel_1 = lv_img_create(parent);
    lv_img_set_src(wheel_1,png_load_path(wheel.png));
    lv_obj_set_height(wheel_1, LV_SIZE_CONTENT);
    lv_obj_set_width(wheel_1, LV_SIZE_CONTENT);   /// 1
    lv_obj_align(wheel_1,LV_ALIGN_CENTER,0,0);
    lv_obj_add_flag(wheel_1, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags

    /*Create another object with the base style and earnings style too*/
    obj_warning1 = lv_obj_create(parent);
    lv_obj_add_style(obj_warning1, &style_base, 0);
    lv_obj_add_style(obj_warning1, &style_warning, 0);
    lv_obj_align_to(obj_warning1,chassis, LV_ALIGN_OUT_TOP_LEFT, 20, 40);

    label1 = lv_label_create(obj_warning1);
    lv_label_set_text(label1, "2316 rpm");
    lv_obj_center(label1);

    obj_warning2 = lv_obj_create(parent);
    lv_obj_add_style(obj_warning2, &style_base, 0);
    lv_obj_add_style(obj_warning2, &style_warning, 0);
    lv_obj_align_to(obj_warning2,chassis, LV_ALIGN_OUT_TOP_RIGHT, -20, 40);

    label2 = lv_label_create(obj_warning2);
    lv_label_set_text(label2, "3316 rpm");
    lv_obj_center(label2);

    obj_warning3 = lv_obj_create(parent);
    lv_obj_add_style(obj_warning3, &style_base, 0);
    lv_obj_add_style(obj_warning3, &style_warning, 0);
    lv_obj_align_to(obj_warning3,chassis, LV_ALIGN_OUT_BOTTOM_LEFT, 20, -40);

    label3 = lv_label_create(obj_warning3);
    lv_label_set_text(label3, "1316 rpm");
    lv_obj_center(label3);

    obj_warning4 = lv_obj_create(parent);
    lv_obj_add_style(obj_warning4, &style_base, 0);
    lv_obj_add_style(obj_warning4, &style_warning, 0);
    lv_obj_align_to(obj_warning4,chassis, LV_ALIGN_OUT_BOTTOM_RIGHT, -20, -40);

    label4 = lv_label_create(obj_warning4);
    lv_label_set_text(label4, "4316 rpm");
    lv_obj_center(label4);

    wave_animation(wheel_1, 2920,200);

}
static void wave_animation(lv_obj_t * TargetObject, uint32_t delay_time,int32_t end_value)
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
static void ofs_set_x_anim(void  * a, lv_coord_t v)
{
    lv_img_set_offset_x(a, v);
    //printf("ofs x = %d\n",v);
}
