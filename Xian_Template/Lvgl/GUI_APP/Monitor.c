#include "Monitor.h"

static void btn_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * btn = lv_event_get_target(e);
    if(code == LV_EVENT_CLICKED) {
        static uint8_t cnt = 0;
        cnt++;

        /*Get the first child of the button which is the label and change its text*/
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        lv_label_set_text_fmt(label, "Button: %d", cnt);
    }
}

/**
 * Create a button with a label and react on click event.
 */
void lv_example_get_started_1(void)
{
    lv_obj_t * btn = lv_btn_create(lv_scr_act());     /*Add a button the current screen*/
    //lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
	lv_obj_set_align(btn,LV_ALIGN_CENTER);
    lv_obj_set_size(btn, 120, 50);                          /*Set its size*/
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_ALL, NULL);           /*Assign a callback to the button*/

    lv_obj_t * label = lv_label_create(btn);          /*Add a label to the button*/
    lv_label_set_text(label, "Button");                     /*Set the labels text*/
    lv_obj_center(label);
}

//dm_motor.bin
//motor_png.png
void lvgl_demo()
{
//	lv_example_get_started_1();
	lv_obj_t* label = lv_label_create(lv_scr_act());
	lv_obj_set_style_text_font(label,&myFont36,LV_STATE_DEFAULT);
	lv_obj_set_pos(label,20,170);
	lv_label_set_text(label,"Ls_Racing车队,车载仪表,田润显,\n\
										上海海能汽车电子有限公司,36号宋体!");
	lv_obj_t* label2 = lv_label_create(lv_scr_act());
	lv_obj_set_style_text_font(label2,&myFont20,LV_STATE_DEFAULT);
	lv_obj_set_pos(label2,20,120);
	//lv_obj_align_to(label2,label,LV_ALIGN_TOP_MID,0,150);
	lv_label_set_text(label2,"Ls_Racing车队,车载仪表,田润显,\n\
											上海海能汽车电子有限公司,20号宋体!");
	
	lv_obj_t * img = lv_img_create(lv_scr_act());
	//lv_obj_center(img);
	lv_obj_set_pos(img,100,300);
	lv_img_set_src(img,"0:/PICTURE/dm_motor.bin");
}

