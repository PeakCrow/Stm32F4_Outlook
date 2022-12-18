#include "about_phone_ui.h"
#include "monitor.h"



static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t s_style_common;
static void About_Phone_In_Ui(lv_obj_t* parent);
#define screen_left_pos 30


void About_Phone_Ui(lv_obj_t *parent)
{
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,"0:/PICTURE/about_phone.bin","0:/PICTURE/about_phone.bin",NULL);
    /* 设置按钮大小 */
    lv_obj_set_size(Imgbtn_MC,400,120);
    /* 设置按钮位置 */
    lv_obj_align_to(Imgbtn_MC,parent,LV_ALIGN_TOP_RIGHT,0,0);
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
		About_Phone_In_Ui(App_Common_Init("关于本机"));
}

static void About_Phone_In_Ui(lv_obj_t* parent)
{
    lv_obj_t * Lvgl_label;
    lv_obj_t * Fatfs_label;
    lv_obj_t * Threadx_label;
    lv_obj_t * Author_label;
    lv_obj_t * Ls_label;
    lv_obj_t * Hadware_label;
    lv_obj_t * Hallib_label;
    lv_obj_t * Monitor_label;
    lv_obj_t * About_line1;
    lv_obj_t * About_line2;
    lv_obj_t * About_line3;

    static lv_point_t about_lines1[] = {
        {5,30},{745,30},{735,45},{735,370},{745,385},
        {5,385},{20,370},{20,45},{5,30}
    };
    static lv_point_t about_lines2[] = {
        {735,45},{20,45},{5,30},{5,385}
    };
    static lv_point_t about_lines3[] = {
        {20,370},{735,370},{745,385},{745,30}
    };

	

    Lvgl_label = lv_label_create(parent);
    lv_label_set_recolor(Lvgl_label,true);
    lv_obj_set_style_text_font(Lvgl_label,&lv_font_montserrat_24,LV_PART_MAIN);
    lv_obj_align(Lvgl_label,LV_ALIGN_TOP_LEFT,screen_left_pos,50);
    lv_label_set_text_fmt(Lvgl_label,"current_lvgl_version is #ff0000 %d.%d.%d#",LVGL_VERSION_MAJOR,LVGL_VERSION_MINOR,LVGL_VERSION_PATCH);

    Fatfs_label = lv_label_create(parent);
    lv_label_set_recolor(Fatfs_label,true);
    lv_obj_set_style_text_font(Fatfs_label,&lv_font_montserrat_24,LV_PART_MAIN);
    lv_obj_align(Fatfs_label,LV_ALIGN_TOP_LEFT,screen_left_pos,50+2*20);
    lv_label_set_text_fmt(Fatfs_label,"current_fatfs_revision_id is #ff0000 %d#",_FATFS);

    Threadx_label = lv_label_create(parent);
    lv_label_set_recolor(Threadx_label,true);
    lv_obj_set_style_text_font(Threadx_label,&lv_font_montserrat_24,LV_PART_MAIN);
    lv_obj_align(Threadx_label,LV_ALIGN_TOP_LEFT,screen_left_pos,50+4*20);
    lv_label_set_text_fmt(Threadx_label,"azure_rtos_threadx_version is #ff0000 %d.%d.%d#",THREADX_MAJOR_VERSION,THREADX_MINOR_VERSION,THREADX_PATCH_VERSION);

    Author_label = lv_label_create(parent);
    lv_label_set_recolor(Author_label,true);
    lv_obj_set_style_text_font(Author_label,&lv_font_montserrat_24,LV_PART_MAIN);
    lv_obj_align(Author_label,LV_ALIGN_TOP_LEFT,screen_left_pos,50+6*20);
    lv_label_set_text_fmt(Author_label,"software_and_hardware_author is #ff0000 runxian.tian#");

    Ls_label = lv_label_create(parent);
    lv_label_set_recolor(Ls_label,true);
    lv_obj_set_style_text_font(Ls_label,&lv_font_montserrat_24,LV_PART_MAIN);
    lv_obj_align(Ls_label,LV_ALIGN_TOP_LEFT,screen_left_pos,50+8*20);
    lv_label_set_text_fmt(Ls_label,"ls_racing_fleet is #00ff00 E01#");

    Hadware_label = lv_label_create(parent);
    lv_label_set_recolor(Hadware_label,true);
    lv_obj_set_style_text_font(Hadware_label,&lv_font_montserrat_24,LV_PART_MAIN);
    lv_obj_align(Hadware_label,LV_ALIGN_TOP_LEFT,screen_left_pos,50+10*20);
    lv_label_set_text_fmt(Hadware_label,"onboard_hardware #ff0000 STM32F407|LCD|FLASH|CAN #\n"
                                        "#ff0000                            MLX90614|ADS1256|WS2812B|EEPROM|SDIO#");
    Hallib_label = lv_label_create(parent);
    lv_label_set_recolor(Hallib_label,true);
    lv_obj_set_style_text_font(Hallib_label,&lv_font_montserrat_24,LV_PART_MAIN);
    lv_obj_align(Hallib_label,LV_ALIGN_TOP_LEFT,screen_left_pos,50+13*20);
    lv_label_set_text_fmt(Hallib_label,"stm32f4xx_cmsis_version is #ff0000 %d.%d.%d.%d#",__STM32F4xx_CMSIS_VERSION_MAIN,__STM32F4xx_CMSIS_VERSION_SUB1,__STM32F4xx_CMSIS_VERSION_SUB2,__STM32F4xx_CMSIS_VERSION_RC);

    Monitor_label = lv_label_create(parent);
    lv_label_set_recolor(Monitor_label,true);
    lv_obj_set_style_text_font(Monitor_label,&lv_font_montserrat_24,LV_PART_MAIN);
    lv_obj_align(Monitor_label,LV_ALIGN_TOP_LEFT,screen_left_pos,42+15*20);
    lv_label_set_text_fmt(Monitor_label,"hardware_version is #ff0000 %d.%d.%d#",HARDWARE_MAJOR_VERSION,HARDWARE_MINOR_VERSION,HARDWARE_PATCH_VERSION);


    About_line1 = lv_line_create(parent);
    lv_obj_align(About_line1,LV_ALIGN_TOP_LEFT,0,0);
    lv_line_set_points(About_line1,about_lines1,sizeof(about_lines1)/sizeof(lv_point_t));
    lv_obj_set_style_line_color(About_line1,lv_palette_main(LV_PALETTE_LIGHT_GREEN),LV_PART_MAIN);
    lv_obj_set_style_line_width(About_line1,2,LV_PART_MAIN);
    lv_obj_set_style_line_rounded(About_line1,true,LV_PART_MAIN);

    About_line2 = lv_line_create(parent);
    lv_obj_align(About_line1,LV_ALIGN_TOP_LEFT,0,0);
    lv_line_set_points(About_line2,about_lines2,sizeof(about_lines2)/sizeof(lv_point_t));
    lv_obj_set_style_line_color(About_line2,lv_palette_main(LV_PALETTE_LIGHT_GREEN),LV_PART_MAIN);
    lv_obj_set_style_line_width(About_line2,2,LV_PART_MAIN);
    lv_obj_set_style_line_rounded(About_line2,true,LV_PART_MAIN);

    About_line3 = lv_line_create(parent);
    lv_obj_align(About_line1,LV_ALIGN_TOP_LEFT,0,0);
    lv_line_set_points(About_line3,about_lines3,sizeof(about_lines3)/sizeof(lv_point_t));
    lv_obj_set_style_line_color(About_line3,lv_palette_main(LV_PALETTE_LIGHT_GREEN),LV_PART_MAIN);
    lv_obj_set_style_line_width(About_line3,2,LV_PART_MAIN);
    lv_obj_set_style_line_rounded(About_line3,true,LV_PART_MAIN);	
}

