#include "vehicle_status.h"
#include "monitor.h"



static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t s_style_common;
static void App_btn_Back_Cb(lv_event_t* e);
static void Vehicle_Status_In_Ui(lv_obj_t * parent);



void Vehicle_Status_Ui(lv_obj_t *parent)
{
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,"0:/PICTURE/vehicle_status.bin","0:/PICTURE/vehicle_status.bin",NULL);
    /* 设置按钮大小 */
    lv_obj_set_size(Imgbtn_MC,400,120);
    /* 设置按钮位置 */
    lv_obj_align_to(Imgbtn_MC,parent,LV_ALIGN_BOTTOM_RIGHT,0,0);
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
         Vehicle_Status_In_Ui(App_Common_Init(" 整车状态 "));
}

static void Vehicle_Status_In_Ui(lv_obj_t * parent)
{

	/* 创建返回按钮 */
	lv_obj_t * App_btn_Back = NULL;
	App_btn_Back = lv_imgbtn_create(parent);
	lv_imgbtn_set_src(App_btn_Back,LV_IMGBTN_STATE_RELEASED,"0:/PICTURE/app_btn.bin","0:/PICTURE/app_btn.bin","0:/PICTURE/app_btn.bin");
	lv_obj_set_size(App_btn_Back,40,40);
	lv_obj_align_to(App_btn_Back,parent,LV_ALIGN_BOTTOM_MID,0,0);
	lv_obj_add_event_cb(App_btn_Back,App_btn_Back_Cb,LV_EVENT_ALL,parent);	
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

