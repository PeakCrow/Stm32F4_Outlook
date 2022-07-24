#include "sys.h"


/* 获取当前活动屏幕的宽高 */
#define scr_act_width() lv_obj_get_width(lv_scr_act())
#define scr_act_height() lv_obj_get_height(lv_scr_act())

static void event_1_btn_cb(lv_event_t* e);


void lvgl_demo(void)
{
	
     lv_obj_t* btn;                                                                                 /* 定义按钮 */
    btn = lv_btn_create(lv_scr_act());                                                              /* 初始化按钮 */
    lv_obj_set_size(btn, scr_act_width() / 4, scr_act_height() / 6);                                /* 设置按钮大小 */
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);                                    					/* 设置按钮位置 */

      lv_obj_t* label;                                                                              /* 定义标签 */
    label = lv_label_create(btn);                                                                   /* 初始化标签 */

    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, LV_PART_MAIN);
    lv_label_set_text(label, "Buttom 0 0");                                                         /* 设置标签文本 */
    lv_obj_center(label);                                                                           /* 设置标签位置 */
    lv_obj_update_layout(label);                                                                    /* 手动更新参数 */
	printf("lvgl_996\n");
	
    //lv_obj_add_event_cb(btn, event_1_btn_cb, LV_EVENT_ALL, label);                                  /* 设置按钮回调 */
}

/**
 * @brief  例1按键回调
 * @param  无
 * @return 无
 */
static void event_1_btn_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);

    static uint8_t pressed_cnt = 0;
    static uint8_t pressing_cnt = 0;
    char buf[15];

    if (LV_EVENT_PRESSED == code) {
        pressed_cnt++;
    }
    else if (LV_EVENT_PRESSING == code) {
        pressing_cnt++;
    }
	printf("btn1call\n");
    lv_snprintf(buf, sizeof(buf), "Buttom %d %d", pressed_cnt, pressing_cnt);
    lv_label_set_text((lv_obj_t*)e->user_data, buf);
}

