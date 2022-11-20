#include "adjust_pedal.h"
#include "monitor.h"
#include <stdio.h>
#include "lv_drv_conf.h"


static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t s_style_common;
static void Adjust_Pedal_In_Ui(lv_obj_t* parent);
static void Forward_Btn_Cb(lv_event_t* e);
static void Reverse_Btn_Cb(lv_event_t* e);
static void sw_event_cb(lv_event_t * e);
static void pos_label_event_cb(lv_event_t *e);
static void scroll_event_cb(lv_event_t * e);
static void driver_selection_cb(lv_event_t * e);
static lv_obj_t * forward_btn;
static lv_obj_t * reverse_btn;


void Adjust_Pedal_Ui(lv_obj_t *parent)
{
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,"E:/Ls_Monitor/LVGL_Monitor/images/adjust_pedal.png","E:/Ls_Monitor/LVGL_Monitor/images/adjust_pedal.png",NULL);
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
static void Imgbtn_MC_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_RELEASED)
        Adjust_Pedal_In_Ui(App_Common_Init("Adjust_Pedal"));
}

static void Adjust_Pedal_In_Ui(lv_obj_t* parent)
{
    lv_obj_t * for_label;
    lv_obj_t * rev_label;
    lv_obj_t * sw_label;
    lv_obj_t * pos_label;
<<<<<<< HEAD
    lv_obj_t * sw;
    lv_obj_t * cont;
=======
>>>>>>> 07fc6a0bc99938368aff3b96bf8f0b7e0a94f81e

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
    lv_obj_add_event_cb(pos_label,pos_label_event_cb,LV_EVENT_ALL,pos_label);
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
    sw = lv_switch_create(parent);
    lv_obj_set_pos(sw,680,40);
    lv_obj_add_state(sw, LV_STATE_CHECKED);
    sw_label = lv_label_create(parent);
    lv_obj_align_to(sw_label,sw,LV_ALIGN_OUT_BOTTOM_MID,-25,0);
    lv_label_set_text(sw_label,"Adjust On");
    lv_obj_add_event_cb(sw, sw_event_cb, LV_EVENT_VALUE_CHANGED, sw_label);

    /* 车手滚轮 */
    cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 150, 150);
    lv_obj_set_pos(cont,480,210);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_event_cb(cont, scroll_event_cb, LV_EVENT_SCROLL, NULL);
    lv_obj_set_style_radius(cont, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(cont, true, 0);
    lv_obj_set_scroll_dir(cont, LV_DIR_VER);
    lv_obj_set_scroll_snap_y(cont, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_scrollbar_mode(cont, LV_SCROLLBAR_MODE_OFF);

    uint8_t i;
    for(i = 1; i < 5; i++) {
        lv_obj_t * btn = lv_btn_create(cont);
        lv_obj_set_width(btn, lv_pct(100));

        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "driver %"LV_PRIu32, i);

        /* 聚焦状态 */
        lv_obj_set_style_width(btn,120,LV_STATE_FOCUSED);
        lv_obj_set_style_bg_color(btn,lv_palette_main(LV_PALETTE_RED),LV_STATE_FOCUSED);
        lv_obj_set_style_bg_color(btn,lv_palette_main(LV_PALETTE_BROWN),LV_STATE_DEFAULT);
        lv_obj_add_event_cb(btn,driver_selection_cb,LV_EVENT_ALL,label);
    }
    /*Update the buttons position manually for first*/
    /* 首先手动更新按钮位置 */
    lv_event_send(cont, LV_EVENT_SCROLL, NULL);
    /*Be sure the fist button is in the middle*/
    /* 确保第一个按钮在中间 */
    lv_obj_scroll_to_view(lv_obj_get_child(cont, 0), LV_ANIM_OFF);
}

static void pos_label_event_cb(lv_event_t *e)
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

static void sw_event_cb(lv_event_t * e)
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

/* 车手滚轮回调函数 */
static void scroll_event_cb(lv_event_t * e)
{
    lv_obj_t * cont = lv_event_get_target(e);

    lv_area_t cont_a;
    lv_obj_get_coords(cont, &cont_a);
    lv_coord_t cont_y_center = cont_a.y1 + lv_area_get_height(&cont_a) / 2;

    lv_coord_t r = lv_obj_get_height(cont) * 7 / 10;
    uint32_t i;
    uint32_t child_cnt = lv_obj_get_child_cnt(cont);


    for(i = 0; i < child_cnt; i++) {
        lv_obj_t * child = lv_obj_get_child(cont, (int32_t)i);
        lv_area_t child_a;
        lv_obj_get_coords(child, &child_a);

        lv_coord_t child_y_center = child_a.y1 + lv_area_get_height(&child_a) / 2;

        lv_coord_t diff_y = child_y_center - cont_y_center;
        diff_y = LV_ABS(diff_y);

        /*Get the x of diff_y on a circle.*/
        lv_coord_t x;
        /*If diff_y is out of the circle use the last point of the circle (the radius)*/
        if(diff_y >= r) {
            x = r;
        }
        else {
            /*Use Pythagoras theorem to get x from radius and y*/
            uint32_t x_sqr = (uint32_t)(r * r - diff_y * diff_y);
            lv_sqrt_res_t res;
            lv_sqrt(x_sqr, &res, 0x8000);   /*Use lvgl's built in sqrt root function*/
            x = r - res.i;
        }

        /*Translate the item by the calculated X coordinate*/
        lv_obj_set_style_translate_x(child, x, 0);

        /*Use some opacity with larger translations*/
        lv_opa_t opa = (lv_opa_t)lv_map(x, 0, r, LV_OPA_TRANSP, LV_OPA_COVER);
        lv_obj_set_style_opa(child, LV_OPA_COVER - opa, 0);
    }
}

/* 车手具体按钮回调函数 */
static void driver_selection_cb(lv_event_t * e)
{
    char label_buf[10] = {0};
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * label = lv_event_get_user_data(e);
    strcpy(label_buf,lv_label_get_text(label));

    if(code == LV_EVENT_PRESSED){
        if(memcmp(label_buf,"driver 1",8) == 0)
            printf("cheshou_1\n");
        else if(memcmp(label_buf,"driver 2",8) == 0)
            printf("cheshou_2\n");
        else if(memcmp(label_buf,"driver 3",8) == 0)
            printf("cheshou_3\n");
        else if(memcmp(label_buf,"driver 4",8) == 0)
            printf("cheshou_4\n");
    }
}
