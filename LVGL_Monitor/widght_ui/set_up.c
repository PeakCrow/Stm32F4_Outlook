#include "set_up.h"
#include "monitor.h"
#include <stdio.h>
//#include "lv_drv_conf.h"


static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t s_style_common;
static void Set_Up_In_Ui(lv_obj_t* parent);



void Set_Up_Ui(lv_obj_t *parent)
{
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
#if enviroment_select == 0
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,NULL,"D:/Ls_Monitor_Lower/LVGL_Monitor/widght_ui/images/set_up.png","D:/Ls_Monitor_Lower/LVGL_Monitor/widght_ui/images/set_up.png");
#else
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,NULL,"E:/Ls_Monitor/LVGL_Monitor/images/set_up.png","E:/Ls_Monitor/LVGL_Monitor/images/set_up.png");
#endif
    /* 设置按钮大小 */
    lv_obj_set_size(Imgbtn_MC,400,120);
    /* 设置按钮位置 */
    lv_obj_align_to(Imgbtn_MC,parent,LV_ALIGN_BOTTOM_LEFT,0,0);
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
         Set_Up_In_Ui(App_Common_Init("Set_Up",App_btn_Back_Cb));
}

static void Set_Up_In_Ui(lv_obj_t *parent)
{
    lv_obj_t * apps_slider1;
    apps_slider1 = lv_slider_create(parent);
    lv_slider_set_value(apps_slider1, 80, LV_ANIM_OFF);
    if(lv_slider_get_mode(apps_slider1) == LV_SLIDER_MODE_RANGE)
        lv_slider_set_left_value(apps_slider1, 0, LV_ANIM_OFF);
    lv_obj_set_width(apps_slider1, 19);
    lv_obj_set_height(apps_slider1, 235);
    lv_obj_align_to(apps_slider1,parent, LV_ALIGN_CENTER,250,29);
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
    lv_obj_set_style_bg_img_src(apps_slider1, "D:/Ls_Monitor_Lower/LVGL_Monitor/widght_ui/images/img_slider.png", LV_PART_INDICATOR | LV_STATE_DEFAULT);

    lv_obj_set_style_opa(apps_slider1, 0, LV_PART_KNOB | LV_STATE_DEFAULT);
}
