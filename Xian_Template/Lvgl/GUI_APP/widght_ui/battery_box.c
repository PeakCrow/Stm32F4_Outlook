#include "battery_box.h"
#include "monitor.h"




static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t s_style_common;
static void Imgbtn_Mc_In_Ui(lv_obj_t * parent);



void Battery_Box_Ui(lv_obj_t *parent)
{
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,NULL,"0:/PICTURE/battery_box.bin","0:/PICTURE/battery_box.bin");
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
static void Imgbtn_MC_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_RELEASED)
         Imgbtn_Mc_In_Ui(App_Common_Init(" 电池箱数据 "));
}

static void Imgbtn_Mc_In_Ui(lv_obj_t * parent)
{
    /* 定义并创建图像按钮 */
    lv_obj_t* battery_bg;
    battery_bg = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_img_set_src(battery_bg,"0:/PICTURE/battery_bg.bin");
    lv_obj_set_width(battery_bg, 55);
    lv_obj_set_height(battery_bg, 125);
    /* 设置按钮位置 */
    lv_obj_align_to(battery_bg,parent,LV_ALIGN_CENTER,0,0);	

    lv_obj_t* battery_bg1;
    battery_bg1 = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_img_set_src(battery_bg1,"0:/PICTURE/particle_1.bin");
    lv_obj_set_width(battery_bg1, 50);
    lv_obj_set_height(battery_bg1, 120);    
    /* 设置按钮位置 */
    lv_obj_align_to(battery_bg1,parent,LV_ALIGN_CENTER,80,0);

    lv_obj_t* battery_bg2;
    battery_bg2 = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_img_set_src(battery_bg2,"0:/PICTURE/particle_2.bin");
    lv_obj_set_width(battery_bg2, 50);
    lv_obj_set_height(battery_bg2, 120);	
    /* 设置按钮位置 */
    lv_obj_align_to(battery_bg2,parent,LV_ALIGN_CENTER,190,0);

    lv_obj_t* battery_bg3;
    battery_bg3 = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_img_set_src(battery_bg3,"0:/PICTURE/particle_3.bin");
    lv_obj_set_width(battery_bg3, 50);
    lv_obj_set_height(battery_bg3, 120);	
    /* 设置按钮位置 */
    lv_obj_align_to(battery_bg3,parent,LV_ALIGN_CENTER,270,0);	
}

