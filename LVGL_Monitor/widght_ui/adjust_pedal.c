#include "adjust_pedal.h"
#include "monitor.h"
#include <stdio.h>
#include "lv_drv_conf.h"


static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t s_style_common;
static void Adjust_Pedal_In_Ui(lv_obj_t* parent);



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
static const char* map[] = {"qianjian","houtui","jilu weizhi",""};
static void Adjust_Pedal_In_Ui(lv_obj_t* parent)
{

    lv_obj_t* _table = lv_tabview_create(parent,LV_DIR_RIGHT,100);
    lv_obj_set_size(_table,lv_obj_get_width(parent)-50,lv_obj_get_height(parent)-100);
    lv_obj_t*first_tab =  lv_tabview_add_tab(_table,"first driver1");
    lv_obj_t*second_tab = lv_tabview_add_tab(_table,"second driver2");
    lv_obj_t*third_tab = lv_tabview_add_tab(_table,"third driver3");


    lv_obj_t* _btnmatrix_first = lv_btnmatrix_create(first_tab);
    lv_obj_set_size(_btnmatrix_first,600,200);
    lv_btnmatrix_set_map(_btnmatrix_first,map);
    lv_btnmatrix_set_one_checked(_btnmatrix_first,true);
    lv_btnmatrix_set_btn_ctrl_all(_btnmatrix_first,LV_BTNMATRIX_CTRL_CHECKABLE);

    lv_obj_t* _btnmatrix_second = lv_btnmatrix_create(second_tab);
    lv_obj_set_size(_btnmatrix_second,600,200);
    lv_btnmatrix_set_map(_btnmatrix_second,map);
    lv_btnmatrix_set_one_checked(_btnmatrix_second,true);
    lv_btnmatrix_set_btn_ctrl_all(_btnmatrix_second,LV_BTNMATRIX_CTRL_CHECKABLE);

    lv_obj_t* _btnmatrix_third = lv_btnmatrix_create(third_tab);
    lv_obj_set_size(_btnmatrix_third,600,200);
    lv_btnmatrix_set_map(_btnmatrix_third,map);
    lv_btnmatrix_set_one_checked(_btnmatrix_third,true);
    lv_btnmatrix_set_btn_ctrl_all(_btnmatrix_third,LV_BTNMATRIX_CTRL_CHECKABLE);


}
