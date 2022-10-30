#include "adjust_pedal.h"
#include "monitor.h"
#include <stdio.h>
#include "lv_drv_conf.h"


static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t s_style_common;
static void Adjust_Pedal_In_Ui(lv_obj_t* parent);
static void Forward_Btn_Cb(lv_event_t* e);
static void Reverse_Btn_Cb(lv_event_t* e);


void Adjust_Pedal_Ui(lv_obj_t *parent)
{
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,"E:/Ls_Monitor/LVGL_Monitor/images/adjust_pedal.png","E:/Ls_Monitor/LVGL_Monitor/images/adjust_pedal.png",NULL);
    /* 设置按钮大小 */
    lv_obj_set_size(Imgbtn_MC,200,120);
    /* 设置按钮位置 */
    lv_obj_align_to(Imgbtn_MC,parent,LV_ALIGN_RIGHT_MID,0,-60);
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
static void Imgbtn_MC_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_RELEASED)
        Adjust_Pedal_In_Ui(App_Common_Init("Adjust_Pedal"));
}

static void Adjust_Pedal_In_Ui(lv_obj_t* parent)
{
    lv_obj_t * forward_btn;
    lv_obj_t * reverse_btn;
    lv_obj_t * for_label;
    lv_obj_t * rev_label;

    forward_btn = lv_btn_create(parent);
    lv_obj_set_size(forward_btn,160,100);
    lv_obj_align_to(forward_btn,parent,LV_ALIGN_CENTER,-150,0);
    for_label = lv_label_create(parent);
    lv_label_set_text(for_label,"FORWARD");
    lv_obj_set_style_text_font(for_label,&lv_font_montserrat_14,LV_PART_MAIN);
    lv_obj_align_to(for_label,forward_btn,LV_ALIGN_CENTER,0,0);
    lv_obj_add_event_cb(forward_btn,Forward_Btn_Cb,LV_EVENT_ALL,NULL);

    reverse_btn = lv_btn_create(parent);
    lv_obj_set_size(reverse_btn,160,100);
    lv_obj_align_to(reverse_btn,parent,LV_ALIGN_CENTER,150,0);
    rev_label = lv_label_create(parent);
    lv_label_set_text(rev_label,"REVERSE");
    lv_obj_set_style_text_font(rev_label,&lv_font_montserrat_14,LV_PART_MAIN);
    lv_obj_align_to(rev_label,reverse_btn,LV_ALIGN_CENTER,0,0);
    lv_obj_add_event_cb(reverse_btn,Reverse_Btn_Cb,LV_EVENT_ALL,NULL);
}

static void Forward_Btn_Cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_PRESSED)
        printf("1--qianjian\n");
    else if(code == LV_EVENT_RELEASED)
        printf("1--houtui\n");

}
static void Reverse_Btn_Cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_PRESSED)
        printf("2--qianjian\n");
    else if(code == LV_EVENT_RELEASED)
        printf("2--houtui\n");
}
