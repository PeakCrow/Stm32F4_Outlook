#include "monitor.h"

#include "widght_ui/motor_control_ui.h"
#include "widght_ui/about_phone_ui.h"
#include "widght_ui/set_up.h"
#include "widght_ui/vehicle_status.h"
#include "widght_ui/battery_box.h"
#include "widght_ui/sensor_ui.h"
#include "widght_ui/adjust_pedal.h"
#include "widght_ui/cool_control.h"


static lv_obj_t* Monitor_Speed_Meter;
//static lv_obj_t *image;
static void lv_example_Monitor_Speed_Meter(void);

#define scr_act_height() lv_obj_get_height(lv_scr_act())
#define scr_act_width()  lv_obj_get_width(lv_scr_act())

static void Monitor_Main_Style(lv_obj_t* Monitor_Speed_Meter);
static void Monitor_Main_label(lv_coord_t weight, lv_coord_t height);
static void App_btn_Back_Cb(lv_event_t* e);


void Gui_Monitor_App()
{

    uint16_t meter_height = 0;
    meter_height = ((unsigned short)(scr_act_height() * 0.8));

    Motor_Control_Ui(lv_scr_act());
    About_Phone_Ui(lv_scr_act());
    Set_Up_Ui(lv_scr_act());
    Vehicle_Status_Ui(lv_scr_act());
    Adjust_Pedal_Ui(lv_scr_act());
    Cool_Control_Ui(lv_scr_act());
    Battery_Box_Ui(lv_scr_act());
    Sensor_Ui(lv_scr_act());
    /* 创建速度盘 */
    /* 暂时将仪表盘放在最下面，以此来屏蔽其他部件 */
    lv_example_Monitor_Speed_Meter();
}
void Monitor_Main_Style(lv_obj_t * Monitor_Speed_Meter)
{
    lv_style_t style_speed_meter;
    //创建普通样式
    //样式初始化
    lv_style_init(&style_speed_meter);
    //设置背景--黑色
    //lv_style_set_bg_color(&style_speed_meter, lv_color_hex(0x000000));
    //设置背景透明度--20
    lv_style_set_bg_opa(&style_speed_meter, 80);
    //设置字体颜色--黄色
    lv_style_set_text_color(&style_speed_meter, lv_color_hex(0xffff00));
    //设置速度盘背景渐变颜色方向--垂直向下
    lv_style_set_bg_grad_dir(&style_speed_meter, LV_GRAD_DIR_VER);
    //设置背景梯度起点属性--渐变从0顶部开始
    lv_style_set_bg_grad_stop(&style_speed_meter, 255);
    //设置背景梯度颜色属性
    lv_style_set_bg_grad_color(&style_speed_meter, lv_palette_main(LV_PALETTE_GREEN));
    //设置边框颜色--蓝色
    lv_style_set_border_color(&style_speed_meter, lv_palette_main(LV_PALETTE_BLUE));
    //设置边框的宽度--3
    lv_style_set_border_width(&style_speed_meter, 3);
    //设置速度盘的轮廓属性--3
    lv_style_set_outline_width(&style_speed_meter, 3);
    //设置轮廓属性的颜色--橙色
    lv_style_set_outline_color(&style_speed_meter, lv_palette_main(LV_PALETTE_ORANGE));
    //设置轮廓间隙属性--3
    lv_style_set_outline_pad(&style_speed_meter, 3);

    //设置速度盘对象的样式
    lv_obj_add_style(Monitor_Speed_Meter, &style_speed_meter, LV_PART_MAIN);
}
void Monitor_Main_label(lv_coord_t weight,lv_coord_t height)
{
    lv_obj_t* label_speed_value;

    //设置速度值文本标签
    label_speed_value = lv_label_create(Monitor_Speed_Meter);
    //设置文本与颜色
    lv_label_set_text(label_speed_value, "Km/h");
    //设置文本对齐方式
    lv_obj_set_pos((lv_obj_t*)(label_speed_value),(short)(weight * 0.45), (short)(height * 0.7));
}
/**
 * @brief 仪表2的回调函数
 * @param indic 不知道是啥？
 * @param value 数据
*/
static void anim_2_indic_cb(lv_meter_indicator_t* indic, int32_t value)
{
    lv_meter_set_indicator_end_value(Monitor_Speed_Meter, indic, value);
}
static void lv_example_Monitor_Speed_Meter(void)
{
    const lv_font_t* font;

    /* 根据屏幕大小设置字体 */
    if (scr_act_width() <= 320)
        font = &lv_font_montserrat_8;
    else if (scr_act_width() < 480)
        font = &lv_font_montserrat_10;
    else
        font = &lv_font_montserrat_24;

    /* 定义并创建仪表 */
    Monitor_Speed_Meter = lv_meter_create(lv_scr_act());
    /* 设置仪表宽度 */
    lv_obj_set_width(Monitor_Speed_Meter, (short)(scr_act_height() * 0.8));
    /* 设置仪表高度 */
    lv_obj_set_height(Monitor_Speed_Meter, (short)(scr_act_height() * 0.8));
    /* 设置仪表位置 */
    lv_obj_center(Monitor_Speed_Meter);
    /* 设置仪表文字字体 */
    lv_obj_set_style_text_font(Monitor_Speed_Meter, font, LV_PART_MAIN);
    /* 删除仪表指针样式 */
    //lv_obj_remove_style(Monitor_Speed_Meter, NULL, LV_PART_INDICATOR);

    /* 设置仪表刻度 */
    /* 添加刻度 */
    lv_meter_scale_t* scale = lv_meter_add_scale(Monitor_Speed_Meter);
    /* 设置小刻度 */
    lv_meter_set_scale_ticks(Monitor_Speed_Meter,
                            scale,
                            41,//小刻度的数量
                            2,//小刻度的宽度
                            (unsigned short)(scr_act_height() / 15),//小刻度的长度
                            lv_palette_main(LV_PALETTE_YELLOW)//小刻度的颜色
                            );
    /* 设置主刻度 */
    lv_meter_set_scale_major_ticks(Monitor_Speed_Meter,
                                    scale,
                                    8,//绘画主刻度的步长
                                    4,//主刻度的宽度
                                    (unsigned short)(scr_act_height() / 13),//主刻度的长度
                                    lv_color_hex3(0xEEE),//主刻度的颜色
                                    scr_act_height() / 40//刻度与标签之间的间隙
                                    );
    /* 设置仪表的角度和仪表的范围 */
    lv_meter_set_scale_range(Monitor_Speed_Meter, scale, 0, 100, 270, 135);
    /* 添加指针 */
    lv_meter_indicator_t * indic = lv_meter_add_needle_line(Monitor_Speed_Meter,
                             scale,
                             3,
                             lv_color_hex3(0xf34),
                             -6
                            );
    /* 设置指针指向的数值 */
    lv_meter_set_indicator_value(Monitor_Speed_Meter,
        indic,
        33
    );
    /* 设置指针 */
    lv_meter_indicator_t* indic1 = lv_meter_add_arc(Monitor_Speed_Meter,
                                                    scale,
                                                    (unsigned short)(scr_act_height() / 45),
                                                    lv_palette_main(LV_PALETTE_RED),//红
                                                    0
                                                    );
    lv_meter_indicator_t* indic2 = lv_meter_add_arc(Monitor_Speed_Meter,
                                                    scale,
                                                    (unsigned short)(scr_act_height() / 45),
                                                    lv_palette_main(LV_PALETTE_GREEN),//绿
                                                    -scr_act_height() / 45
                                                    );
    lv_meter_indicator_t* indic3 = lv_meter_add_arc(Monitor_Speed_Meter,
                                                    scale,
                                                    (unsigned short)(scr_act_height() / 45),
                                                    lv_palette_main(LV_PALETTE_BLUE),//蓝
                                                    -(short)(scr_act_height() / 22.5)
                                                    );





    /* 设置指针动画 */
    lv_anim_t a;
    /* 初始化动画对象 */
    lv_anim_init(&a);
    /* 设置动画函数 */
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t)anim_2_indic_cb);
    /* 设置动画的起始值和结束值 */
    lv_anim_set_values(&a, 0, 100);
    /* 重复前延迟，默认为0(禁用)[ms] */
    lv_anim_set_repeat_delay(&a, 100);
    /* 延迟后在回放，默认为0(禁用)[ms] */
    lv_anim_set_playback_delay(&a, 100);
    /* 重复的数量，默认数值为1，LV_ANIM_REPEAT_INFINITE用于无限重复 */
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    /* 设置动画时间长度 */
    lv_anim_set_time(&a, 2000);
    /* 准备好后，按此持续时间倒放动画，默认为0(禁用)[ms] */
    lv_anim_set_playback_time(&a, 500);
    /* 设置动画的对象 */
    lv_anim_set_var(&a, indic1);
    /* 开始动画 */
    lv_anim_start(&a);


    /* 设置动画的时间长度 */
    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_set_var(&a, indic2);
    lv_anim_start(&a);


    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 2000);
    lv_anim_set_var(&a, indic3);
    lv_anim_start(&a);

    //创建样式
    //Monitor_Main_Style(Monitor_Speed_Meter);
    //创建速度值标签
    //Monitor_Main_label((short)(scr_act_height() * 0.8),(short)(scr_act_height() * 0.8));
}

lv_obj_t* App_Common_Init(const char *title)
{
    lv_obj_t * App_btn_Back = NULL;
    lv_obj_t * App_Title = NULL;
    lv_obj_t * parent = NULL;
    //const char btn_source[] = "E:/Ls_Monitor/LVGL_Monitor/images/app_btn.png";

    /* 创建返回按钮 */
    parent = lv_obj_create(lv_scr_act());
    lv_obj_set_size(parent,800,480);
    App_btn_Back = lv_imgbtn_create(parent);

    lv_imgbtn_set_src(App_btn_Back,LV_IMGBTN_STATE_RELEASED,"0:/PICTURE/app_btn.bin","0:/PICTURE/app_btn.bin","0:/PICTURE/app_btn.bin");
    lv_obj_set_size(App_btn_Back,40,40);
    lv_obj_align_to(App_btn_Back,parent,LV_ALIGN_BOTTOM_MID,0,0);
    lv_obj_add_event_cb(App_btn_Back,App_btn_Back_Cb,LV_EVENT_ALL,parent);
    /* 创建标题文本 */
    App_Title = lv_label_create(parent);
    lv_theme_apply(App_Title);
	lv_obj_set_style_text_font(App_Title,&myFont36,LV_STATE_DEFAULT);
    lv_label_set_text(App_Title,title);
    lv_obj_align_to(App_Title,parent,LV_ALIGN_TOP_MID,0,0);
    /* 创建返回按钮文本 */

    return parent;
}

static void App_btn_Back_Cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* parent = lv_event_get_user_data(e);
    switch ((uint8_t)code) {
        case LV_EVENT_RELEASED:
            {
                lv_obj_del(parent);
				lv_timer_del(RealtimeMotorpos_timer);
            }
            break;
    }
}


