#include "adjust_pedal.h"
#include "monitor.h"



static void Imgbtn_MC_cb(lv_event_t * e);
static void Adjust_Pedal_In_Ui(lv_obj_t* parent);
static void Forward_Btn_Cb(lv_event_t* e);
static void Reverse_Btn_Cb(lv_event_t* e);
static void sw_event_cb(lv_event_t * e);
static void pos_label_event_cb(lv_event_t *e);
static lv_obj_t * forward_btn;
static lv_obj_t * reverse_btn;
static double NumberConuts = 0;





void Adjust_Pedal_Ui(lv_obj_t *parent)
{
	static lv_style_t s_style_common;	
	
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,"0:/PICTURE/adjust_pedal.bin","0:/PICTURE/adjust_pedal.bin",NULL);
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
        Adjust_Pedal_In_Ui(App_Common_Init(" Adjust_Pedal "));
}
static void Adjust_Pedal_In_Ui(lv_obj_t* parent)
{
    lv_obj_t * for_label;
    lv_obj_t * rev_label;
    lv_obj_t * sw_label;
	lv_obj_t * pos_label;
	
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
    lv_style_set_width(&style_warning, 250);
    lv_style_set_height(&style_warning,LV_SIZE_CONTENT);
	
	/* 电机实时位置标签 */
	pos_label = lv_label_create(parent);
	lv_obj_align_to(pos_label,sw_label,LV_ALIGN_OUT_BOTTOM_MID,-50,100);
	lv_obj_align(pos_label,LV_ALIGN_CENTER,0,0);
	lv_label_set_text_fmt(pos_label,"  Pedal_Pos:%.2f",NumberConuts);	
	lv_obj_add_style(pos_label,&style_warning,0);
	lv_obj_add_event_cb(pos_label,pos_label_event_cb,LV_EVENT_ALL,pos_label);
		
    /* 正转按钮 */
    forward_btn = lv_btn_create(parent);
    lv_obj_set_size(forward_btn,160,100);
    lv_obj_align_to(forward_btn,parent,LV_ALIGN_CENTER,-150,-100);
    for_label = lv_label_create(parent);
    lv_label_set_text(for_label,"FORWARD");
    lv_obj_set_style_bg_color(forward_btn,lv_palette_main(LV_PALETTE_YELLOW),LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(for_label,&lv_font_montserrat_24,LV_PART_MAIN);
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
    lv_obj_set_style_text_font(rev_label,&lv_font_montserrat_24,LV_PART_MAIN);
    lv_obj_align_to(rev_label,reverse_btn,LV_ALIGN_CENTER,0,0);
    lv_obj_add_event_cb(reverse_btn,Reverse_Btn_Cb,LV_EVENT_ALL,pos_label);
    lv_obj_add_style(reverse_btn,&style_pr,LV_STATE_PRESSED);
    lv_obj_add_style(reverse_btn,&style_def,0);	
	
	/* 正反转屏蔽按钮 */
    lv_obj_t * sw = lv_switch_create(parent);
    lv_obj_set_pos(sw,650,40);
    lv_obj_add_state(sw, LV_STATE_CHECKED);
    sw_label = lv_label_create(parent);
    lv_obj_align_to(sw_label,sw,LV_ALIGN_OUT_BOTTOM_MID,-25,0);
    lv_label_set_text(sw_label,"Adjust On");
    lv_obj_add_event_cb(sw, sw_event_cb, LV_EVENT_VALUE_CHANGED, sw_label);
}
static void pos_label_event_cb(lv_event_t *e)
{
	lv_event_code_t code = lv_event_get_code(e);	
	lv_obj_t * pos_label = lv_event_get_user_data(e);
	
	/* 读取闭环电机的实时位置，也就是电机自上电/使能起所转过的角度 */
	uint8_t Postition[] = {0xe0,0x36};
	static uint8_t Pos[5] = {0};
	static int32_t PosQueue = 0;	
	
	if(code == LV_EVENT_PRESSING ){		
		comSendBuf(COM3,Postition,2);
		for(int i = 0;i < 5;i++)
			comGetChar(COM3,&Pos[i]);		
		if(Pos[0] == 0xe0)
			PosQueue = ((Pos[1]<<24) | (Pos[2]<<16) | (Pos[3] << 8) | Pos[4]);

		/* 更新标签文本值 */			
		NumberConuts = PosQueue / 65535.00;
		lv_label_set_text_fmt(pos_label,"  Pedal_Pos:%.2f",NumberConuts);

	}else if(code == LV_EVENT_RELEASED){
		App_Printf("%d ",PosQueue);
        App_I2C_EE_BufferWrite(Pos,0x00,5);
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
static void Forward_Btn_Cb(lv_event_t* e)
{
	uint8_t stop_motor[] = {0xe0,0xf7};
	uint8_t Forward_rotation[] = {0xe0,0xf6,0x7e};/* 电机以127档速度正转 */
    lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * pos_label = lv_event_get_user_data(e);
	
    if(code == LV_EVENT_PRESSED){        
		comSendBuf(COM3,Forward_rotation,3);
	}
    else if(code == LV_EVENT_RELEASED){
		comSendBuf(COM3,stop_motor,2);	
        lv_event_send(pos_label,LV_EVENT_RELEASED,NULL);
    }
	else if(code == LV_EVENT_PRESSING){
		lv_event_send(pos_label,LV_EVENT_PRESSING,NULL);
	}	

}
static void Reverse_Btn_Cb(lv_event_t* e)
{
	uint8_t stop_motor[] = {0xe0,0xf7};
	uint8_t Forward_rotation[] = {0xe0,0xf6,0xfe};/* 电机以127档速度反转 */
    lv_event_code_t code = lv_event_get_code(e);
	lv_obj_t * pos_label = lv_event_get_user_data(e);
	
    if(code == LV_EVENT_PRESSED){
		comSendBuf(COM3,Forward_rotation,3);		
	}
    else if(code == LV_EVENT_RELEASED){
		comSendBuf(COM3,stop_motor,2);	
        lv_event_send(pos_label,LV_EVENT_RELEASED,NULL);
    }
	else if(code == LV_EVENT_PRESSING){
		lv_event_send(pos_label,LV_EVENT_PRESSING,NULL);
	}
}


