#include "cool_control.h"
#include "monitor.h"
#include <stdio.h>
#include "lv_drv_conf.h"


static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t s_style_common;




void Cool_Control_Ui(lv_obj_t *parent)
{
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,NULL,"E:/Ls_Monitor/LVGL_Monitor/images/cool_control.png",NULL);
    /* 设置按钮大小 */
    lv_obj_set_size(Imgbtn_MC,200,120);
    /* 设置按钮位置 */
    lv_obj_align_to(Imgbtn_MC,parent,LV_ALIGN_RIGHT_MID,0,60);
    /* 普通样式将透明度设置为50 */
    static lv_style_prop_t tr_prop[] = {LV_STYLE_TRANSLATE_Y,
                                        LV_STYLE_IMG_RECOLOR_OPA,
                                        LV_STYLE_PROP_INV};
    static lv_style_transition_dsc_t tr;
    lv_style_transition_dsc_init(&tr, tr_prop, lv_anim_path_linear,
    100, 0, NULL);
    /* 添加按钮释放风格 */
    lv_img_set_antialias(Imgbtn_MC,true);
    lv_obj_set_style_transition(Imgbtn_MC, &tr, LV_STATE_DEFAULT);
    lv_style_init(&s_style_common);
    lv_style_set_opa(&s_style_common,LV_OPA_50);
    //lv_obj_set_ext_click_area(Imgbtn_MC,-200);
    /* 普通样式将图片按钮缩小%20 */
    lv_obj_set_style_img_recolor(Imgbtn_MC,lv_palette_main(LV_PALETTE_BLUE),LV_INDEV_STATE_RELEASED);
    //lv_obj_set_style_translate_y(Imgbtn_MC,50,LV_INDEV_STATE_PRESSED);
    lv_style_set_transform_angle(&s_style_common,450);
    //lv_style_set_transform_zoom(&s_style_common,128);
    /* 设置按钮按下时的样式 */
    lv_obj_add_style(Imgbtn_MC,&s_style_common,LV_STATE_PRESSED);
    /* 设置按钮回调 */
    lv_obj_add_event_cb(Imgbtn_MC,Imgbtn_MC_cb,LV_EVENT_ALL,NULL);
}
static void Imgbtn_MC_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_RELEASED)
         App_Common_Init("Cool_Control");
}
