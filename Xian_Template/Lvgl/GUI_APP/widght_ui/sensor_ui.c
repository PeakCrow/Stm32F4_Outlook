#include "sensor_ui.h"
#include "monitor.h"



static void Imgbtn_MC_cb(lv_event_t * e);
static lv_style_t style_indic;
static void Sensor_In_Ui(lv_obj_t * parent);
static void set_temp(void * bar,int32_t temp);
static void event_cb(lv_event_t * e);
static void App_btn_Back_Cb(lv_event_t* e);
static lv_timer_t * Realtime_Sensor_timer;
static void Realtime_Sensor_Cb(lv_timer_t * e);
lv_obj_t * analog1,*analog2,*analog3,*analog4;
lv_chart_series_t * ser,*ser1,*ser2,*ser3;




void Sensor_Ui(lv_obj_t *parent)
{
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,NULL,"0:/PICTURE/sensor.bin","0:/PICTURE/sensor.bin");
    /* 设置按钮大小 */
    lv_obj_set_size(Imgbtn_MC,200,120);
    /* 设置按钮位置 */
    lv_obj_align_to(Imgbtn_MC,parent,LV_ALIGN_LEFT_MID,0,60);
    /*Create a transition animation on width transformation and recolor.*/
    static lv_style_prop_t tr_prop[] = {LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_IMG_RECOLOR_OPA, 0};
    static lv_style_transition_dsc_t tr;
    lv_style_transition_dsc_init(&tr, tr_prop, lv_anim_path_linear, 200, 0, NULL);

    static lv_style_t style_def;
    lv_style_init(&style_def);
    lv_style_set_text_color(&style_def, lv_color_white());
    lv_style_set_transition(&style_def, &tr);
    lv_obj_add_style(Imgbtn_MC,&style_indic,LV_STATE_PRESSED);

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
				lv_timer_del(Realtime_Sensor_timer);
				lv_obj_del(parent);
			}
			break;
	}
}
static void Imgbtn_MC_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_RELEASED)
         Sensor_In_Ui(App_Common_Init((" 传感器数据 "),App_btn_Back_Cb));
}
/*************************应用代码************************************************/
/* 图表回调函数，使用label来显示数据 */
static void event_chart_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * chart = lv_event_get_target(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_invalidate(chart);
    }
    if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        lv_coord_t * s = lv_event_get_param(e);
        *s = LV_MAX(*s, 20);
    }
    else if(code == LV_EVENT_DRAW_POST_END) {
        uint32_t id = lv_chart_get_pressed_point(chart);
        if(id == LV_CHART_POINT_NONE) 
			return;

        //LV_LOG_USER("Selected point %d", (int)id);

        lv_chart_series_t * ser = lv_chart_get_series_next(chart, NULL);
        while(ser) {
            lv_point_t p;
            lv_chart_get_point_pos_by_id(chart, ser, (uint16_t)id, &p);

            lv_coord_t * y_array = lv_chart_get_y_array(chart, ser);
            lv_coord_t value = y_array[id];

            char buf[16];
            lv_snprintf(buf, sizeof(buf), LV_SYMBOL_DUMMY"%d", value);

            lv_draw_rect_dsc_t draw_rect_dsc;
            lv_draw_rect_dsc_init(&draw_rect_dsc);
            draw_rect_dsc.bg_color = lv_color_white();
            draw_rect_dsc.bg_opa = LV_OPA_50;
            draw_rect_dsc.radius = 3;
            draw_rect_dsc.bg_img_src = buf;
            draw_rect_dsc.bg_img_recolor = lv_color_black();

            lv_area_t a;
            a.x1 = chart->coords.x1 + p.x - 10;
            a.x2 = chart->coords.x1 + p.x + 10;
            a.y1 = chart->coords.y1 + p.y - 10;
            a.y2 = chart->coords.y1 + p.y - 10;

            lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);
            lv_draw_rect(draw_ctx, &draw_rect_dsc, &a);

            ser = lv_chart_get_series_next(chart, ser);
        }
    }
    else if(code == LV_EVENT_RELEASED) {
        lv_obj_invalidate(chart);
    }

}


static void Sensor_In_Ui(lv_obj_t * parent)
{
    lv_obj_t * analog1_label,*analog2_label,*analog3_label,*analog4_label;
    lv_obj_t * bar;

    {
        analog1 = lv_chart_create(parent);
        lv_obj_set_size(analog1, 550, 80);
        lv_obj_set_pos(analog1, 0, 20);
        lv_chart_set_range(analog1, LV_CHART_AXIS_PRIMARY_Y, -1000, 6000);
        lv_chart_set_type(analog1,LV_CHART_TYPE_LINE);
        lv_chart_set_update_mode(analog1,LV_CHART_UPDATE_MODE_CIRCULAR);
        lv_chart_set_div_line_count(analog1,2,50);
        lv_chart_set_update_mode(analog1,LV_CHART_UPDATE_MODE_SHIFT);
        lv_obj_set_style_line_color(analog1,lv_palette_main(LV_PALETTE_PINK),LV_PART_MAIN);
        lv_obj_set_style_size(analog1, 0, LV_PART_INDICATOR);
        lv_obj_refresh_ext_draw_size(analog1);
        lv_obj_add_event_cb(analog1,event_chart_cb,LV_EVENT_ALL,NULL);
        analog1_label = lv_label_create(parent);
        lv_obj_align_to(analog1_label,analog1,LV_ALIGN_OUT_RIGHT_MID,10,0);
        lv_label_set_text(analog1_label,"value");
    }
    {
        analog2 = lv_chart_create(parent);
        lv_obj_set_size(analog2, 550, 80);
        lv_obj_set_pos(analog2, 0, 110);
        lv_chart_set_range(analog2, LV_CHART_AXIS_PRIMARY_Y, -1000, 6000);
        lv_chart_set_type(analog2,LV_CHART_TYPE_LINE);
        lv_chart_set_update_mode(analog2,LV_CHART_UPDATE_MODE_CIRCULAR);
        lv_chart_set_div_line_count(analog2,2,50);
        lv_chart_set_update_mode(analog2,LV_CHART_UPDATE_MODE_SHIFT);
        lv_obj_set_style_line_color(analog2,lv_palette_main(LV_PALETTE_PINK),LV_PART_MAIN);
        lv_obj_set_style_size(analog2, 0, LV_PART_INDICATOR);
        lv_obj_add_event_cb(analog2,event_chart_cb,LV_EVENT_ALL,NULL);
        analog2_label = lv_label_create(parent);
        lv_obj_align_to(analog2_label,analog2,LV_ALIGN_OUT_RIGHT_MID,10,0);
        lv_label_set_text(analog2_label,"value");
    }
    {
        analog3 = lv_chart_create(parent);
        lv_obj_set_size(analog3, 550, 80);
        lv_obj_set_pos(analog3, 0, 200);
        lv_chart_set_range(analog3, LV_CHART_AXIS_PRIMARY_Y, -1000, 6000);
        lv_chart_set_type(analog3,LV_CHART_TYPE_LINE);
        lv_chart_set_update_mode(analog3,LV_CHART_UPDATE_MODE_CIRCULAR);
        lv_chart_set_div_line_count(analog3,2,50);
        lv_chart_set_update_mode(analog3,LV_CHART_UPDATE_MODE_SHIFT);
        lv_obj_set_style_line_color(analog3,lv_palette_main(LV_PALETTE_PINK),LV_PART_MAIN);
        lv_obj_set_style_size(analog3, 0, LV_PART_INDICATOR);
        lv_obj_add_event_cb(analog3,event_chart_cb,LV_EVENT_ALL,NULL);
        analog3_label = lv_label_create(parent);
        lv_obj_align_to(analog3_label,analog3,LV_ALIGN_OUT_RIGHT_MID,10,0);
        lv_label_set_text(analog3_label,"value");
    }
    {
        analog4 = lv_chart_create(parent);
        lv_obj_set_size(analog4, 550, 80);
        lv_obj_set_pos(analog4, 0, 290);
        lv_chart_set_range(analog4, LV_CHART_AXIS_PRIMARY_Y, -1000, 6000);
        lv_chart_set_type(analog4,LV_CHART_TYPE_LINE);
        lv_chart_set_update_mode(analog4,LV_CHART_UPDATE_MODE_CIRCULAR);
        lv_chart_set_div_line_count(analog4,2,50);
        lv_chart_set_update_mode(analog4,LV_CHART_UPDATE_MODE_SHIFT);
        lv_obj_set_style_line_color(analog4,lv_palette_main(LV_PALETTE_PINK),LV_PART_MAIN);
        lv_obj_set_style_size(analog4, 0, LV_PART_INDICATOR);
        lv_obj_add_event_cb(analog4,event_chart_cb,LV_EVENT_ALL,NULL);
        analog4_label = lv_label_create(parent);
        lv_obj_align_to(analog4_label,analog4,LV_ALIGN_OUT_RIGHT_MID,10,0);
        lv_label_set_text(analog4_label,"value");
    }

    /* 添加数据序列到图表 */
    ser = lv_chart_add_series(analog1,
                              lv_palette_main(LV_PALETTE_RED),
                              LV_CHART_AXIS_PRIMARY_Y);/* 左轴 */
    ser1 = lv_chart_add_series(analog2,
                              lv_palette_main(LV_PALETTE_GREEN),
                              LV_CHART_AXIS_PRIMARY_Y);/* 左轴 */
    ser2 = lv_chart_add_series(analog3,
                              lv_palette_main(LV_PALETTE_CYAN),
                              LV_CHART_AXIS_PRIMARY_Y);/* 左轴 */
    ser3 = lv_chart_add_series(analog4,
                              lv_palette_main(LV_PALETTE_BROWN),
                              LV_CHART_AXIS_PRIMARY_Y);/* 左轴 */
    /* 设置数据线上的点数 */
    lv_chart_set_point_count(analog1, 50);
    lv_chart_set_point_count(analog2, 50);
    lv_chart_set_point_count(analog3, 50);
    lv_chart_set_point_count(analog4, 50);
    for(uint16_t i = 0;i < 50;i++){
        lv_chart_set_next_value(analog1,ser,(short)lv_rand(0,5000));
        lv_chart_set_next_value(analog2,ser1,(short)lv_rand(0,5000));
        lv_chart_set_next_value(analog3,ser2,(short)lv_rand(0,5000));
        lv_chart_set_next_value(analog4,ser3,(short)lv_rand(0,5000));
    }
    lv_chart_refresh(analog1);
    lv_chart_refresh(analog2);
    lv_chart_refresh(analog3);
    lv_chart_refresh(analog4);


    lv_style_init(&style_indic);
    lv_style_set_bg_opa(&style_indic, LV_OPA_COVER);
    lv_style_set_bg_color(&style_indic, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_indic, lv_palette_main(LV_PALETTE_BLUE));
    lv_style_set_bg_grad_dir(&style_indic, LV_GRAD_DIR_VER);

    bar = lv_bar_create(parent);
    lv_obj_add_style(bar, &style_indic, LV_PART_INDICATOR);
    lv_obj_set_size(bar, 40, 400);
    lv_obj_align(bar,LV_ALIGN_RIGHT_MID,-50,0);
    lv_bar_set_range(bar, -20, 80);
    lv_obj_add_event_cb(bar,event_cb,LV_EVENT_DRAW_PART_END,NULL);

//    lv_anim_t a;
//    lv_anim_init(&a);
//    lv_anim_set_exec_cb(&a, set_temp);
//    lv_anim_set_time(&a, 3000);
//    lv_anim_set_playback_time(&a, 3000);
//    lv_anim_set_var(&a, bar);
//    lv_anim_set_values(&a, -20, 80);
//    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
//    lv_anim_start(&a);

	/* 创建一个LVGL定时器定时添加chart数据 */
	//Realtime_Sensor_timer = lv_timer_create(Realtime_Sensor_Cb,1000,NULL);
	//lv_timer_set_cb(Realtime_Sensor_timer,Realtime_Sensor_Cb);	


}
static void Realtime_Sensor_Cb(lv_timer_t * e)
{
	lv_chart_set_next_value(analog1,ser,(short)lv_rand(0,5000));
	lv_chart_set_next_value(analog2,ser1,(short)lv_rand(0,5000));
	lv_chart_set_next_value(analog3,ser2,(short)lv_rand(0,5000));
	lv_chart_set_next_value(analog4,ser3,(short)lv_rand(0,5000));

}

static void set_temp(void * bar,int32_t temp)
{
    lv_bar_set_value(bar,temp,LV_ANIM_ON);
}
/* 温度计的数值显示回调函数 */
static void event_cb(lv_event_t * e)
{
    /* 通过参数`LV_EVENT_DRAW_PART_BEGIN/END`获得部分区域描述 */
    lv_obj_draw_part_dsc_t * dsc = lv_event_get_draw_part_dsc(e);
    if(dsc->part != LV_PART_INDICATOR) return;

    lv_obj_t * obj = lv_event_get_target(e);

    lv_draw_label_dsc_t label_dsc;
    lv_draw_label_dsc_init(&label_dsc);
    label_dsc.font = &lv_font_montserrat_14;

    char buf[16];
    lv_snprintf(buf, sizeof(buf), "%d", (int)lv_bar_get_value(obj));
//    lv_snprintf(buf, sizeof(buf), "%d", (int)9);

    lv_point_t txt_size;

    lv_txt_get_size(&txt_size, buf, label_dsc.font, label_dsc.letter_space, label_dsc.line_space, LV_COORD_MAX,
                    label_dsc.flag);

    lv_area_t txt_area;
    /*If the indicator is long enough put the text inside on the right*/
    if(lv_area_get_width(dsc->draw_area) > txt_size.x + 20) {
        /* 将个位数数值更偏向左边一点 */
        //if((int)lv_bar_get_value(obj) < 10 && (int)lv_bar_get_value(obj) > 0)
//            txt_area.x2 = dsc->draw_area->x2 - 16;
//        else
            txt_area.x2 = dsc->draw_area->x2 - 12;
        txt_area.x1 = txt_area.x2 - txt_size.x + 1;
        label_dsc.color = lv_color_white();
    }
    /*If the indicator is still short put the text out of it on the right*/
    else {
        txt_area.x1 = dsc->draw_area->x2 + 15;
        txt_area.x2 = txt_area.x1 + txt_size.x - 15;
        label_dsc.color = lv_color_black();
    }

    //txt_area.y1 = dsc->draw_area->y1 + (lv_area_get_height(dsc->draw_area) - txt_size.y) / 2;
    txt_area.y1 = dsc->draw_area->y1 + 10/*+ (lv_area_get_height(dsc->draw_area) - txt_size.y) / 2*/;
    txt_area.y2 = txt_area.y1 + txt_size.y - 1;

    lv_draw_label(dsc->draw_ctx, &label_dsc, &txt_area, buf, NULL);
}



