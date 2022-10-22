#include "adjust_pedal.h"
#include "monitor.h"
#include <stdio.h>
#include "lv_drv_conf.h"


static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t s_style_common;
static void Adjust_Pedal_In_Ui(lv_obj_t* parent);
static void Padel_Control_cb(lv_event_t* e);


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
    lv_obj_t * pedal_arc = lv_arc_create(parent);
    lv_obj_set_size(pedal_arc,lv_pct(60),lv_pct(60));
    lv_arc_set_range(pedal_arc,0,100);
    lv_arc_set_value(pedal_arc,50);
    lv_obj_align_to(pedal_arc,NULL,LV_ALIGN_CENTER,100,0);
    lv_arc_set_mode(pedal_arc,LV_ARC_MODE_SYMMETRICAL);
    lv_obj_add_event_cb(pedal_arc,Padel_Control_cb,LV_EVENT_ALL,NULL);

}
static void Padel_Control_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    if(code == LV_EVENT_PRESSING){
        printf("value changed already--");
        printf("%d\n",lv_arc_get_value(obj));
    }
}
