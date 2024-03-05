#include "motor_control_ui.h"
#include "monitor.h"
#include <stdio.h>

typedef enum _indicate_pos
{
    label_right = 0,
    label_left
}indicate_pos;
static void Imgbtn_MC_cb(lv_event_t * e);
static void Motor_Control_In_Ui(lv_obj_t *parent);
static void Three_Circular_Ui(lv_obj_t *parent,
                              lv_coord_t x,lv_coord_t y,
                              indicate_pos);
static void Three_Indicate_Boar(lv_obj_t *_parent,
                                const lv_point_t _points[]);
static void Three_Indicate_Lab(lv_obj_t *_parent);
static void Motor_Data_Indicate_Lab(lv_obj_t *_parent);

static const lv_point_t line_points1[] = { {210, 38}, {310, 38},
                                    {310,79},{235,79},{310,79},
                                    {310,119},{235,119},{310,119},
                                    {310, 160},{210,160}};

static const lv_point_t line_points2[] = { {210, 248}, {310, 248},
                                    {310,289},{235,289},{310,289},
                                    {310,329},{235,329},{310,329},
                                    {310, 370},{210,370}};

static const lv_point_t line_points3[] = { {540, 38}, {440, 38},
                                    {440,79},{515,79},{440,79},
                                    {440,119},{515,119},{440,119},
                                    {440, 160},{540,160}};

static const lv_point_t line_points4[] = { {540, 248}, {440, 248},
                                    {440,289},{515,289},{440,289},
                                    {440,329},{515,329},{440,329},
                                    {440, 370},{540,370}};

static lv_obj_t* motors_data[12] = {0};

void Motor_Control_Ui(lv_obj_t *parent)
{
    static lv_style_t s_style_common;
    /* 定义并创建图像按钮 */
    lv_obj_t* Imgbtn_MC;
    Imgbtn_MC = lv_imgbtn_create(parent);
    /* 设置按钮释放时的图像 */
    lv_imgbtn_set_src(Imgbtn_MC,LV_STATE_DEFAULT,NULL,png_load_path(motor_control.png),png_load_path(motor_control.png));
    /* 设置按钮大小 */
    lv_obj_set_size(Imgbtn_MC,400,120);
    /* 设置按钮位置 */
    lv_obj_align_to(Imgbtn_MC,parent,LV_ALIGN_TOP_LEFT,0,0);
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
        Motor_Control_In_Ui(App_Common_Init("Motor_Control",App_btn_Back_Cb));
}

static void Motor_Control_In_Ui(lv_obj_t *parent)
{
    Three_Indicate_Lab(parent);
    /* 使用4个三色环控件，显示电机温度、扭矩、转速 */
    Three_Circular_Ui(parent,80,50,label_right);
    Three_Indicate_Boar(parent,line_points1);

    Three_Circular_Ui(parent,570,50,label_left);
    Three_Indicate_Boar(parent,line_points2);

    Three_Circular_Ui(parent,80,260,label_right);
    Three_Indicate_Boar(parent,line_points3);

    Three_Circular_Ui(parent,570,260,label_left);
    Three_Indicate_Boar(parent,line_points4);
    Motor_Data_Indicate_Lab(parent);
}

static void Motor_Data_Indicate_Lab(lv_obj_t *_parent)
{
    for(uint8_t i = 0;i < 12;i++)
    {
        motors_data[i] = lv_label_create(_parent);
        lv_label_set_recolor(motors_data[i],true);
        lv_obj_set_style_text_color(motors_data[i],
                                    lv_color_hex(0x008787),
                                    LV_PART_MAIN);
        lv_obj_set_style_text_font(motors_data[i],
                                   &lv_font_montserrat_22,
                                   LV_PART_MAIN);
        if(i % 3 == 0)
        {
            lv_label_set_text(motors_data[i],"1550");
        }else if(i % 3 == 1)
        {
            lv_label_set_text(motors_data[i],"241");
        }else if(i % 3 == 2)
        {
            lv_label_set_text(motors_data[i],"25");
        }
    }
    lv_obj_set_pos(motors_data[0],250,50);
    lv_obj_align_to(motors_data[1],motors_data[0],
                    LV_ALIGN_OUT_BOTTOM_RIGHT,0,15);
    lv_obj_align_to(motors_data[2],motors_data[1],
                    LV_ALIGN_OUT_BOTTOM_RIGHT,0,15);
    lv_obj_align_to(motors_data[3],motors_data[2],
                    LV_ALIGN_OUT_BOTTOM_RIGHT,0,105);
    lv_obj_align_to(motors_data[4],motors_data[3],
                    LV_ALIGN_OUT_BOTTOM_RIGHT,0,15);
    lv_obj_align_to(motors_data[5],motors_data[4],
                    LV_ALIGN_OUT_BOTTOM_RIGHT,0,15);

    lv_obj_set_pos(motors_data[6],450,50);
    lv_obj_align_to(motors_data[7],motors_data[6],
                    LV_ALIGN_OUT_BOTTOM_LEFT,0,15);
    lv_obj_align_to(motors_data[8],motors_data[7],
                    LV_ALIGN_OUT_BOTTOM_LEFT,0,15);
    lv_obj_align_to(motors_data[9],motors_data[8],
                    LV_ALIGN_OUT_BOTTOM_LEFT,0,105);
    lv_obj_align_to(motors_data[10],motors_data[9],
                    LV_ALIGN_OUT_BOTTOM_LEFT,0,15);
    lv_obj_align_to(motors_data[11],motors_data[10],
                    LV_ALIGN_OUT_BOTTOM_LEFT,0,15);
}
static void Three_Indicate_Lab(lv_obj_t *_parent)
{
    lv_obj_t *circular_label[6];
    for(uint8_t i = 0;i < 6;i++)
    {
        circular_label[i] = lv_label_create(_parent);
        if(i % 3 == 0)
        {
            lv_label_set_text(circular_label[i],"SPEED");
        }else if(i % 3 == 1)
        {
            lv_label_set_text(circular_label[i],"TORQUE");
        }else if(i % 3 == 2)
        {
            lv_label_set_text(circular_label[i],"TEMP");
        }
    }
    lv_obj_set_pos(circular_label[0],350,50);
    lv_obj_align_to(circular_label[1],circular_label[0],
                    LV_ALIGN_OUT_BOTTOM_MID,0,25);
    lv_obj_align_to(circular_label[2],circular_label[1],
                    LV_ALIGN_OUT_BOTTOM_MID,0,25);
    lv_obj_align_to(circular_label[3],circular_label[2],
                    LV_ALIGN_OUT_BOTTOM_MID,0,110);
    lv_obj_align_to(circular_label[4],circular_label[3],
                    LV_ALIGN_OUT_BOTTOM_MID,0,25);
    lv_obj_align_to(circular_label[5],circular_label[4],
                    LV_ALIGN_OUT_BOTTOM_MID,0,25);
}
static void Three_Indicate_Boar(lv_obj_t *_parent,
                                const lv_point_t _points[])
{
    lv_obj_t * indicate_line;
    indicate_line = lv_line_create(_parent);
    lv_line_set_points(indicate_line,_points,10);
    lv_obj_set_style_line_width(indicate_line,5,
                                LV_PART_MAIN);
    lv_obj_set_style_line_rounded(indicate_line, true,
                                  LV_PART_MAIN);
}
static void Three_Circular_Ui(lv_obj_t *parent,
                              lv_coord_t x,lv_coord_t y,indicate_pos _indicate_pos)
{
    lv_obj_t * speed_arc,*torque_arc,*temp_arc;

    temp_arc = lv_arc_create(parent);
    lv_obj_clear_flag(temp_arc,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(temp_arc,150,100);
    lv_arc_set_range(temp_arc,0,100);
    lv_arc_set_value(temp_arc,50);
    lv_obj_remove_style(temp_arc,NULL,LV_PART_KNOB);
    lv_obj_set_style_arc_color(temp_arc,lv_color_hex(0xcd8b00),LV_PART_INDICATOR);
    lv_obj_set_pos(temp_arc,x,y);


    torque_arc = lv_arc_create(parent);
    lv_obj_clear_flag(torque_arc,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(torque_arc,150+30,
                    100+30);
    lv_arc_set_range(torque_arc,0,100);
    lv_arc_set_value(torque_arc,60);
    lv_obj_remove_style(torque_arc,NULL,LV_PART_KNOB);
    lv_obj_set_style_arc_color(torque_arc,lv_color_hex(0x7fc241),LV_PART_INDICATOR);
    lv_obj_align_to(torque_arc,temp_arc,LV_ALIGN_CENTER,0,0);


    speed_arc = lv_arc_create(parent);
    lv_obj_clear_flag(speed_arc,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(speed_arc,lv_obj_get_width(temp_arc)+60,
                    lv_obj_get_height(temp_arc)+60);
    lv_arc_set_range(speed_arc,0,100);
    lv_arc_set_value(speed_arc,70);
    lv_obj_remove_style(speed_arc,NULL,LV_PART_KNOB);
    lv_obj_set_style_arc_color(speed_arc,lv_color_hex(0xff473d),LV_PART_INDICATOR);
    lv_obj_align_to(speed_arc,temp_arc,LV_ALIGN_CENTER,0,0);


    lv_obj_t * indicate_arc;
    indicate_arc = lv_arc_create(parent);
    lv_obj_clear_flag(indicate_arc,LV_OBJ_FLAG_CLICKABLE);
    lv_obj_set_size(indicate_arc,lv_obj_get_width(speed_arc),
                    lv_obj_get_height(speed_arc));
    lv_obj_remove_style(indicate_arc,NULL,LV_PART_KNOB);
    lv_obj_set_style_arc_color(indicate_arc,lv_color_hex(0x7fc241),
                               LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(indicate_arc, 5,
                           LV_PART_INDICATOR);
    lv_obj_set_style_arc_width(indicate_arc, 5,
                           LV_PART_MAIN);
    lv_obj_set_style_arc_color(indicate_arc,lv_color_hex(0x333333)
                               ,LV_PART_INDICATOR);
    if(_indicate_pos == label_right)
    {
        lv_arc_set_start_angle(indicate_arc,310);
        lv_arc_set_end_angle(indicate_arc,50);
        lv_arc_set_bg_angles(indicate_arc,310,50);
        lv_obj_align_to(indicate_arc,temp_arc,
                        LV_ALIGN_CENTER,30,0);
    }else if(_indicate_pos == label_left)
    {
        lv_arc_set_start_angle(indicate_arc,130);
        lv_arc_set_end_angle(indicate_arc,230);
        lv_arc_set_bg_angles(indicate_arc,130,230);
        lv_obj_align_to(indicate_arc,speed_arc,
                        LV_ALIGN_CENTER,-30,0);
    }else
    {
        printf("invalid label pos!!\n");
    }
}
