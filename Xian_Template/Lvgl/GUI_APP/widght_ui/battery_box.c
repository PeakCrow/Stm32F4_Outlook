#include "battery_box.h"
#include "monitor.h"




static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t s_style_common;
static void Battery_Box_In_Ui(lv_obj_t * parent);
static void App_btn_Back_Cb(lv_event_t* e);


static void ofs_set_y_anim(void * img, int32_t v)
{
    lv_img_set_offset_y(img, v);
}

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
		Battery_Box_In_Ui(App_Common_Init((" 电池箱数据 "),App_btn_Back_Cb));
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


static void Battery_Box_In_Ui(lv_obj_t * parent)
{
    lv_obj_t * ui_Image_Particle1;
    lv_obj_t * ui_Image_Particle2;
    lv_obj_t * ui_Image_Particle3;
    lv_obj_t * ui_Image_Battery_BG;


    ui_Image_Battery_BG = lv_img_create(parent);
	lv_img_set_src(ui_Image_Battery_BG,"0:/PICTURE/battery_bg.bin");
    lv_obj_set_width(ui_Image_Battery_BG, LV_SIZE_CONTENT);   /// 1
    lv_obj_set_height(ui_Image_Battery_BG, LV_SIZE_CONTENT);    /// 1
    lv_obj_set_align(ui_Image_Battery_BG, LV_ALIGN_CENTER);
    lv_obj_set_style_radius(ui_Image_Battery_BG, 10, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_color(ui_Image_Battery_BG, lv_color_hex(0x438DB5), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_width(ui_Image_Battery_BG, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_shadow_spread(ui_Image_Battery_BG, 0, LV_PART_MAIN | LV_STATE_DEFAULT);

	
    ui_Image_Particle1 = lv_img_create(parent);   
    lv_img_set_src(ui_Image_Particle1,"0:/PICTURE/particle_1.bin");
    lv_obj_set_height(ui_Image_Particle1, LV_SIZE_CONTENT);
    lv_obj_set_width(ui_Image_Particle1, LV_SIZE_CONTENT);   /// 1
	lv_obj_align_to(ui_Image_Particle1,ui_Image_Battery_BG,LV_ALIGN_CENTER,0,0);
	lv_obj_set_style_shadow_opa(ui_Image_Particle1, 255, LV_STATE_DEFAULT);




    ui_Image_Particle2 = lv_img_create(parent);
    lv_img_set_src(ui_Image_Particle2,"0:/PICTURE/particle_2.bin");
    lv_obj_set_height(ui_Image_Particle2, LV_SIZE_CONTENT);
    lv_obj_set_width(ui_Image_Particle2, LV_SIZE_CONTENT);   /// 1
    lv_obj_align_to(ui_Image_Particle2,ui_Image_Battery_BG,LV_ALIGN_CENTER,0,0);
	lv_obj_set_style_shadow_opa(ui_Image_Particle2, 255, LV_STATE_DEFAULT );




    ui_Image_Particle3 = lv_img_create(parent);
    lv_img_set_src(ui_Image_Particle3,"0:/PICTURE/particle_3.bin");
    lv_obj_set_height(ui_Image_Particle3, LV_SIZE_CONTENT);
   	lv_obj_set_width(ui_Image_Particle3, LV_SIZE_CONTENT);   /// 1
    lv_obj_align_to(ui_Image_Particle3,ui_Image_Battery_BG,LV_ALIGN_CENTER,0,0);
	lv_obj_set_style_shadow_opa(ui_Image_Particle3, 255, LV_STATE_DEFAULT);




    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, ui_Image_Particle1);
    lv_anim_set_time(&a, 6920);
    lv_anim_set_exec_cb(&a, ofs_set_y_anim);
    lv_anim_set_values(&a, 0, -110);
    lv_anim_set_path_cb(&a, lv_anim_path_linear);
    lv_anim_set_delay(&a,0);
    lv_anim_set_playback_time(&a, 0);
    lv_anim_set_playback_delay(&a,0);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&a, 0);
    lv_anim_set_early_apply(&a, false);
    lv_anim_start(&a);


    lv_anim_t b;
    lv_anim_init(&b);
    lv_anim_set_var(&b, ui_Image_Particle2);
    lv_anim_set_time(&b, 9920);
    lv_anim_set_exec_cb(&b, ofs_set_y_anim);
    lv_anim_set_values(&b, 0, -110);
    lv_anim_set_path_cb(&b, lv_anim_path_linear);
    lv_anim_set_delay(&b,0);
    lv_anim_set_playback_time(&b, 0);
    lv_anim_set_playback_delay(&b,0);
    lv_anim_set_repeat_count(&b, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_repeat_delay(&b, 0);
    lv_anim_set_early_apply(&b, false);
    lv_anim_start(&b);


	lv_anim_t c;
	lv_anim_init(&c);
	lv_anim_set_var(&c, ui_Image_Particle3);
	lv_anim_set_time(&c, 2920);
	lv_anim_set_exec_cb(&c, ofs_set_y_anim);
	lv_anim_set_values(&c, 0, -110);
	lv_anim_set_path_cb(&c, lv_anim_path_linear);
	lv_anim_set_delay(&c,0);
	lv_anim_set_playback_time(&c, 0);
	lv_anim_set_playback_delay(&c,0);
	lv_anim_set_repeat_count(&c, LV_ANIM_REPEAT_INFINITE);
	lv_anim_set_repeat_delay(&c, 0);
	lv_anim_set_early_apply(&c, false);
	lv_anim_start(&c);

    
   	
}





