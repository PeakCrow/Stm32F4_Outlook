#include "motor_control_ui.h"
#include "monitor.h"
#include <stdio.h>
//#include "lv_drv_conf.h"


static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t s_style_common;
static void Motor_Control_In_Ui(lv_obj_t *parent);


static void ofs_y_anim(void * img, int32_t v)
{
    lv_img_set_offset_y(img, v);
    printf("%d\n",v);
    //lv_img_set_pivot(img,400,240);
    //lv_img_set_angle(img,100);
}
static void ofs_x_anim(void * img, int32_t v)
{
    lv_img_set_offset_x(img, v);
    printf("%d\n",v);
    //lv_img_set_pivot(img,400,240);
    //lv_img_set_angle(img,100);
}
void Motor_Control_Ui(lv_obj_t *parent)
{
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,NULL,"D:/Ls_Monitor_Lower/LVGL_Monitor/widght_ui/images/motor_control.png","D:/Ls_Monitor_Lower/LVGL_Monitor/widght_ui/images/motor_control.png");
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
    lv_obj_t * ui_Image_Particle1,* ui_Image_Particle2;


    lv_obj_t * spinner = lv_spinner_create(parent, 1000, 80);
    lv_obj_set_size(spinner, 400, 400);
    lv_obj_center(spinner);

    /* 电池soc */
    ui_Image_Particle1 = lv_img_create(parent);
    lv_img_set_src(ui_Image_Particle1,"D:/Ls_Monitor_Lower/LVGL_Monitor/widght_ui/images/horzi_direction.png");
    lv_obj_set_height(ui_Image_Particle1, LV_SIZE_CONTENT);
    lv_obj_set_width(ui_Image_Particle1, LV_SIZE_CONTENT);   /// 1
    //lv_obj_align_to(ui_Image_Particle1,NULL, LV_ALIGN_LEFT_MID,50,20);
    lv_obj_align(ui_Image_Particle1,LV_ALIGN_CENTER,0,0);
    lv_obj_add_flag(ui_Image_Particle1, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    //lv_obj_clear_flag(ui_Image_Particle1, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    //lv_obj_set_style_blend_mode(ui_Image_Particle1, LV_BLEND_MODE_ADDITIVE, LV_PART_MAIN | LV_STATE_DEFAULT);

    ui_Image_Particle2 = lv_img_create(parent);
    lv_img_set_src(ui_Image_Particle2,"D:/Ls_Monitor_Lower/LVGL_Monitor/widght_ui/images/verti_direction.png");
    lv_obj_set_height(ui_Image_Particle2, LV_SIZE_CONTENT);
    lv_obj_set_width(ui_Image_Particle2, LV_SIZE_CONTENT);   /// 1
    //lv_obj_align_to(ui_Image_Particle1,NULL, LV_ALIGN_LEFT_MID,50,20);
    lv_obj_align(ui_Image_Particle2,LV_ALIGN_CENTER,150,0);
    lv_obj_add_flag(ui_Image_Particle2, LV_OBJ_FLAG_ADV_HITTEST);     /// Flags
    //lv_obj_clear_flag(ui_Image_Particle2, LV_OBJ_FLAG_SCROLLABLE);      /// Flags
    //lv_obj_set_style_blend_mode(ui_Image_Particle2, LV_BLEND_MODE_ADDITIVE, LV_PART_MAIN | LV_STATE_DEFAULT);
}
