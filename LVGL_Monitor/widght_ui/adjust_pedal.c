﻿#include "adjust_pedal.h"
#include "monitor.h"
#include <stdio.h>


/**********************样式变量必须做为全局变量******************************/
static lv_style_t s_style_common;
static lv_style_t style_radio;
static lv_style_t style_radio_chk;
/************************************************************************/

/*************************回调函数*****************************************/
static void Imgbtn_MC_cb(lv_event_t * e);
static void Adjust_Pedal_In_Ui(lv_obj_t* parent);
static void Forward_Btn_Cb(lv_event_t* e);
static void Reverse_Btn_Cb(lv_event_t* e);
static void Shild_Sw_Cb(lv_event_t * e);
static void Pos_Label_Cb(lv_event_t *e);
static void Radio_Btn_Cb(lv_event_t * e);
static void Record_Btn_Cb(lv_event_t *e);
static void Zero_Btn_Cb(lv_event_t * e);
/************************************************************************/

/***********************全局对象-obj***************************************/
static lv_obj_t * forward_btn;
static lv_obj_t * reverse_btn;
static uint32_t active_index_2 = 0;/* 车手选择是不是也要做到掉电保存里面？ */
/************************************************************************/


void Adjust_Pedal_Ui(lv_obj_t *parent)
{
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,NULL,png_load_path(adjust_pedal.png),png_load_path(adjust_pedal.png));
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
        Adjust_Pedal_In_Ui(App_Common_Init("Adjust_Pedal",App_btn_Back_Cb));
    	//App_Common_Init("Adjust_Pedal",App_btn_Back_Cb);
}

static void Adjust_Pedal_In_Ui(lv_obj_t* parent)
{
    lv_obj_t * for_label;
    lv_obj_t * rev_label;
    lv_obj_t * sw_label;
    lv_obj_t * pos_label;
    lv_obj_t * shild_switch;
    lv_obj_t * record_button;
    lv_obj_t * record_label;
    lv_obj_t * zero_button;
    lv_obj_t * zero_label;

    char buf[12];


    /* 样式配置 */
    /*Properties to transition*/
    static lv_style_prop_t props[] = {
        LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT, LV_STYLE_TEXT_LETTER_SPACE, 0
    };

    /*Transition descriptor when going back to the default state.
     *Add some delay to be sure the press transition is visible even if the press was very short*/
    static lv_style_transition_dsc_t transition_dsc_def;
    lv_style_transition_dsc_init(&transition_dsc_def, props, lv_anim_path_overshoot, 250, 100, NULL);

    /*Transition descriptor when going to pressed state.
     *No delay, go to presses state immediately*/
    static lv_style_transition_dsc_t transition_dsc_pr;
    lv_style_transition_dsc_init(&transition_dsc_pr, props, lv_anim_path_ease_in_out, 250, 0, NULL);

    /*Add only the new transition to he default state*/
    static lv_style_t style_def;
    lv_style_init(&style_def);
    lv_style_set_transition(&style_def, &transition_dsc_def);

    /*Add the transition and some transformation to the presses state.*/
    static lv_style_t style_pr;
    lv_style_init(&style_pr);
    lv_style_set_transform_width(&style_pr, 20);
    lv_style_set_transform_height(&style_pr, -20);
    lv_style_set_text_letter_space(&style_pr, 20);
    lv_style_set_transition(&style_pr, &transition_dsc_pr);

    /*Set only the properties that should be different*/
    static lv_style_t style_warning;
    lv_style_init(&style_warning);
    lv_style_set_bg_color(&style_warning, lv_palette_main(LV_PALETTE_YELLOW));
    lv_style_set_border_color(&style_warning, lv_palette_darken(LV_PALETTE_YELLOW, 3));
    lv_style_set_text_color(&style_warning, lv_palette_darken(LV_PALETTE_YELLOW, 4));
    lv_style_set_border_width(&style_warning, 2);
    lv_style_set_radius(&style_warning, 10);
    lv_style_set_shadow_width(&style_warning, 10);
    lv_style_set_shadow_ofs_y(&style_warning, 5);
    lv_style_set_shadow_opa(&style_warning, LV_OPA_50);
    lv_style_set_width(&style_warning, 170);
    lv_style_set_height(&style_warning,LV_SIZE_CONTENT);

    /* 电机实时位置标签 */
    pos_label = lv_label_create(parent);
    lv_obj_align_to(pos_label,parent,LV_ALIGN_OUT_BOTTOM_MID,-50,100);
    lv_obj_align(pos_label,LV_ALIGN_CENTER,0,0);
    lv_label_set_text_fmt(pos_label,"  Pedal_Pos:%d",9999);
    lv_obj_add_event_cb(pos_label,Pos_Label_Cb,LV_EVENT_ALL,pos_label);
    lv_obj_add_style(pos_label,&style_warning,0);

    /* 正转按钮 */
    forward_btn = lv_btn_create(parent);
    lv_obj_set_size(forward_btn,160,100);
    lv_obj_align_to(forward_btn,parent,LV_ALIGN_CENTER,-150,-100);
    for_label = lv_label_create(parent);
    lv_label_set_text(for_label,"FORWARD");
    lv_obj_set_style_bg_color(forward_btn,lv_palette_main(LV_PALETTE_YELLOW),LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(for_label,&lv_font_montserrat_14,LV_PART_MAIN);
    lv_obj_align_to(for_label,forward_btn,LV_ALIGN_CENTER,0,0);
    lv_obj_add_event_cb(forward_btn,Forward_Btn_Cb,LV_EVENT_ALL,pos_label);
    lv_obj_add_style(forward_btn,&style_pr,LV_STATE_PRESSED);
    lv_obj_add_style(forward_btn,&style_def,0);


    /* 反转按钮 */
    reverse_btn = lv_btn_create(parent);
    lv_obj_set_size(reverse_btn,160,100);
    lv_obj_align_to(reverse_btn,parent,LV_ALIGN_CENTER,150,-100);
    rev_label = lv_label_create(parent);
    lv_label_set_text(rev_label,"REVERSE");
    lv_obj_set_style_bg_color(reverse_btn,lv_palette_main(LV_PALETTE_GREEN),LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(rev_label,&lv_font_montserrat_14,LV_PART_MAIN);
    lv_obj_align_to(rev_label,reverse_btn,LV_ALIGN_CENTER,0,0);
    lv_obj_add_event_cb(reverse_btn,Reverse_Btn_Cb,LV_EVENT_ALL,pos_label);
    lv_obj_add_style(reverse_btn,&style_pr,LV_STATE_PRESSED);
    lv_obj_add_style(reverse_btn,&style_def,0);

    /* 屏蔽按钮 */
    shild_switch = lv_switch_create(parent);
    lv_obj_set_pos(shild_switch,680,40);
    lv_obj_add_state(shild_switch, LV_STATE_CHECKED);
    sw_label = lv_label_create(parent);
    lv_obj_align_to(sw_label,shild_switch,LV_ALIGN_OUT_BOTTOM_MID,-25,0);
    lv_label_set_text(sw_label,"Adjust On");
    lv_obj_add_event_cb(shild_switch, Shild_Sw_Cb, LV_EVENT_VALUE_CHANGED, sw_label);

    /* 车手选择 */
    lv_style_init(&style_radio);
    lv_style_set_radius(&style_radio, LV_RADIUS_CIRCLE);

    lv_style_init(&style_radio_chk);
    lv_style_set_bg_img_src(&style_radio_chk, NULL);
    lv_style_set_bg_color(&style_radio_chk,lv_palette_main(LV_PALETTE_RED));

    lv_obj_t *cont2 = lv_obj_create(parent);
    lv_obj_set_flex_flow(cont2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_size(cont2, 150, 200);
    lv_obj_set_pos(cont2,600,200);
    lv_obj_add_event_cb(cont2, Radio_Btn_Cb, LV_EVENT_CLICKED, &active_index_2);

    for(uint8_t i = 0; i < 3; i++) {
        lv_snprintf(buf, sizeof(buf), "Driver %d", (int)i + 1);

        lv_obj_t * obj = lv_checkbox_create(cont2);
        lv_checkbox_set_text(obj, buf);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_EVENT_BUBBLE);
        lv_obj_add_style(obj, &style_radio, LV_PART_INDICATOR);
        lv_obj_add_style(obj, &style_radio_chk, LV_PART_INDICATOR | LV_STATE_CHECKED);

    }
    /* 每次点击页面进来都是上一次的车手选择 */
    lv_obj_add_state(lv_obj_get_child(cont2,(int32_t)active_index_2), LV_STATE_CHECKED);

    /* 车手位置更新记录按钮 */
    record_button = lv_btn_create(parent);
    lv_obj_set_size(record_button,lv_pct(10),lv_pct(15));
    lv_obj_align_to(record_button,pos_label,LV_ALIGN_OUT_LEFT_MID,-150,20);
    record_label = lv_label_create(parent);
    lv_label_set_text(record_label,"RECORD");
    lv_obj_align_to(record_label,record_button,LV_ALIGN_CENTER,0,0);
    lv_obj_add_style(record_button,&style_pr,LV_STATE_PRESSED);
    lv_obj_add_style(record_button,&style_def,0);
    lv_obj_set_style_bg_color(record_button,lv_palette_main(LV_PALETTE_RED),LV_STATE_DEFAULT);
    lv_obj_add_event_cb(record_button,Record_Btn_Cb,LV_EVENT_CLICKED,&active_index_2);

    /* 踏板0位置标定按钮 */
    zero_button = lv_btn_create(parent);
    lv_obj_set_size(zero_button,lv_pct(10),lv_pct(15));
    lv_obj_align_to(zero_button,record_button,LV_ALIGN_OUT_BOTTOM_MID,0,50);
    zero_label = lv_label_create(parent);
    lv_label_set_text(zero_label,"ZERO");
    lv_obj_align_to(zero_label,zero_button,LV_ALIGN_CENTER,0,0);
    lv_obj_add_style(zero_button,&style_pr,LV_STATE_PRESSED);
    lv_obj_add_style(zero_button,&style_def,0);
    lv_obj_set_style_bg_color(zero_button,lv_palette_main(LV_PALETTE_BROWN),LV_STATE_DEFAULT);
    lv_obj_add_event_cb(zero_button,Zero_Btn_Cb,LV_EVENT_CLICKED,NULL);

    /* 车手位置label显示 */
    lv_style_t style_shadow;
    lv_style_init(&style_shadow);
    lv_style_set_text_opa(&style_shadow,LV_OPA_30);
    lv_style_set_text_color(&style_shadow,lv_color_black());

    for(uint8_t i = 0;i < 3;i++){
        lv_obj_t * shadow_label = lv_label_create(parent);
        /* 此API会导致模拟器崩溃。 */
        //lv_obj_add_style(shadow_label,&style_shadow,0);
        lv_obj_t * driverpos_label = lv_label_create(parent);
        lv_obj_set_style_text_font(driverpos_label,&lv_font_montserrat_24,LV_PART_MAIN);
        lv_label_set_text_fmt(driverpos_label,"Driver %d Pos %d ",i+1,50 + (i+1) * 10);
        lv_obj_add_style(driverpos_label,&style_shadow,0);
        lv_obj_align_to(driverpos_label,pos_label,LV_ALIGN_OUT_BOTTOM_MID,0,(i+1)*40);
        lv_label_set_text(shadow_label,lv_label_get_text(driverpos_label));
        lv_obj_set_style_text_font(shadow_label,&lv_font_montserrat_24,LV_PART_MAIN);
        lv_obj_align_to(shadow_label,driverpos_label,LV_ALIGN_TOP_LEFT,2,2);
    }

}
/* 电机位置label显示回调函数 */
static void Pos_Label_Cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * pos_label = lv_event_get_user_data(e);
    static double i = -99999;

    if(code == LV_EVENT_PRESSING){        
        i += 0.11;
        lv_label_set_text_fmt(pos_label,"  Pedal_Pos:%.2f",i);
    }
    else if(code == LV_EVENT_RELEASED){
        printf("zhenfananniuanxia\n");
    }
}
/* 屏蔽按钮回调函数 */
static void Shild_Sw_Cb(lv_event_t * e)
{
    lv_obj_t * sw = lv_event_get_target(e);
    lv_obj_t * sw_label = lv_event_get_user_data(e);
    if(lv_obj_has_state(sw, LV_STATE_CHECKED)) {/* 关闭状态 */
        lv_label_set_text(sw_label,"Adjust On");
        lv_obj_add_flag(forward_btn,LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(reverse_btn,LV_OBJ_FLAG_CLICKABLE);
    }
    else {/* 默认打开 */
        lv_label_set_text(sw_label,"Adjust Off");
        lv_obj_clear_flag(reverse_btn,LV_OBJ_FLAG_CLICKABLE);
        lv_obj_clear_flag(forward_btn,LV_OBJ_FLAG_CLICKABLE);
    }
}
/* 正转按钮回调函数 */
static void Forward_Btn_Cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * pos_label = lv_event_get_user_data(e);

    if(code == LV_EVENT_PRESSED)
        printf("1--qianjian\n");
    else if(code == LV_EVENT_RELEASED){
        printf("1--houtui\n");
        lv_event_send(pos_label,LV_EVENT_RELEASED,NULL);
    }
    else if(code == LV_EVENT_PRESSING){
        lv_event_send(pos_label,LV_EVENT_PRESSING,NULL);
    }

}
/* 反转按钮回调函数 */
static void Reverse_Btn_Cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * pos_label = lv_event_get_user_data(e);

    if(code == LV_EVENT_PRESSED)
        printf("2--qianjian\n");
    else if(code == LV_EVENT_RELEASED){
        printf("2--houtui\n");
        lv_event_send(pos_label,LV_EVENT_RELEASED,NULL);
    }
    else if(code == LV_EVENT_PRESSING){
        lv_event_send(pos_label,LV_EVENT_PRESSING,NULL);
    }
}
/* 车手选择回调函数 */
static void Radio_Btn_Cb(lv_event_t * e)
{
    uint32_t * active_id = lv_event_get_user_data(e);
    lv_obj_t * cont = lv_event_get_current_target(e);
    lv_obj_t * act_cb = lv_event_get_target(e);
    lv_obj_t * old_cb = lv_obj_get_child(cont, (int32_t)*active_id);

    /*Do nothing if the container was clicked*/
    if(act_cb == cont) return;

    /*Uncheck the previous radio button*/
    lv_obj_clear_state(old_cb, LV_STATE_CHECKED);
    /*Uncheck the current radio button*/
    lv_obj_add_state(act_cb, LV_STATE_CHECKED);

    *active_id = lv_obj_get_index(act_cb);

    //LV_LOG_USER("Selected radio buttons: %d",  (int)active_index_2);
    printf("select radio buttons:%d \n",(int)active_index_2+1);
}
/* 车手位置记录更新按钮 */
static void Record_Btn_Cb(lv_event_t *e)
{
    (void)e;
    printf("Driver %d Pos is updated!\n",active_index_2+1);
}
/* 踏板0位置标定 */
static void Zero_Btn_Cb(lv_event_t * e)
{
    (void)e;
    printf("Driver 0 Pos is Cal\n");
}
