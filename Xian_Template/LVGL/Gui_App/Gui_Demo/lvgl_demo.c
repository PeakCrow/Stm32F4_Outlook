#include "sys.h"

static void btn_event_cb(lv_event_t * e);

void lvgl_demo(void)
{

	
	lv_obj_t * btn = lv_btn_create(lv_scr_act());                   /*Add a button to the current screen*/
	lv_obj_set_pos(btn, 10, 10);                                    /*Set its position*/
	lv_obj_set_size(btn, 100, 50);                                  /*Set its size*/
	lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL); /*Assign a callback to the button*/

	lv_obj_t * label = lv_label_create(btn);                        /*Add a label to the button*/
	lv_label_set_text(label, "Button");                             /*Set the labels text*/
	lv_obj_center(label);                                           /*Align the label to the center*/
}

void btn_event_cb(lv_event_t * e)
{
	  printf("Clicked\n\r");
}
