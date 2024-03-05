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
static void lv_example_Monitor_Speed_Meter(void);

#define scr_act_height() lv_obj_get_height(lv_scr_act())
#define scr_act_width()  lv_obj_get_width(lv_scr_act())

static void Monitor_Main_Style(lv_obj_t* Monitor_Speed_Meter);
//static void App_btn_Back_Cb(lv_event_t* e);
static void lv_example_btnmatrix(lv_obj_t *parent);
static void lv_gui_app_screen(void);


void Gui_Monitor_App()
{
    /* 记得需要做按键矩阵的开机输入密码界面 */
    lv_obj_set_style_bg_color(lv_scr_act(),lv_color_hex(0x123456),LV_STATE_DEFAULT);
    lv_example_btnmatrix(lv_scr_act());
}
void lv_gui_app_screen()
{
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
    static lv_style_t style_speed_meter;
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
    const lv_font_t* font = &myFont36;

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
    //创建样式
    Monitor_Main_Style(Monitor_Speed_Meter);

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
                                    scr_act_height() / 15//刻度与标签之间的间隙
                                    );
    /* 设置仪表的角度和仪表的范围 */
    lv_meter_set_scale_range(Monitor_Speed_Meter, scale, 0, 99, 270, 135);
    /* 添加指针 */
    lv_meter_indicator_t * indic = lv_meter_add_needle_line(Monitor_Speed_Meter,
                             scale,6,lv_color_hex3(0xf34),-6);
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
    //创建速度值标签
    lv_obj_t* label_speed_value;

    //设置速度值文本标签
    label_speed_value = lv_label_create(Monitor_Speed_Meter);
    //设置文本与颜色
    lv_label_set_text(label_speed_value, " 33\nkm/h");
    lv_obj_set_style_text_color(label_speed_value,lv_color_hex(0x7fc241),LV_PART_MAIN);
    //设置文本对齐方式
    lv_obj_align_to(label_speed_value,Monitor_Speed_Meter,LV_ALIGN_BOTTOM_MID,0,0);
    
#if LVGL_THREAD_X_ANIM == 1
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
    
    lv_anim_set_time(&a, 3000);
    lv_anim_set_playback_time(&a, 3000);
    lv_anim_set_var(&a, indic);
    lv_anim_start(&a);
#endif
    //创建样式
    Monitor_Main_Style(Monitor_Speed_Meter);
}

/**
 * @brief 创建界面公共函数，因为不同的界面的返回按键删除回调函数不同
 *          所以将按键删除函数作为参数传给界面创建公共函数
 * @param title:界面标题  App_btn_Back_Cb:界面删除函数
 * @param 返回一个满屏幕的容器，在此容器中创建子控件
*/

lv_obj_t* App_Common_Init(const char *title,App_btn_Back_Cb_Ptr App_btn_Back_Cb)
{    
    lv_obj_t * App_Title = NULL;
    lv_obj_t * parent = NULL;
    //const char btn_source[] = "E:/Ls_Monitor/LVGL_Monitor/images/app_btn.png";
    
    parent = lv_obj_create(lv_scr_act());
    lv_obj_set_style_bg_color(parent,lv_color_hex(0x1cbac8),LV_STATE_DEFAULT);
    lv_obj_set_size(parent,800,480);

    
    /* 创建标题文本 */
    App_Title = lv_label_create(parent);
    lv_theme_apply(App_Title);
    lv_obj_set_style_text_font(App_Title,&myFont20,LV_STATE_DEFAULT);
    lv_label_set_text(App_Title,title);
    lv_obj_align_to(App_Title,parent,LV_ALIGN_TOP_MID,0,0);
    
    /* 创建返回按钮 */
    lv_obj_t * App_btn_Back = NULL;
    App_btn_Back = lv_imgbtn_create(parent);
    lv_imgbtn_set_src(App_btn_Back,LV_IMGBTN_STATE_RELEASED,png_load_path(app_btn.bin),
                        png_load_path(app_btn.bin),png_load_path(app_btn.bin));
    lv_obj_set_size(App_btn_Back,40,40);
    lv_obj_align_to(App_btn_Back,parent,LV_ALIGN_BOTTOM_MID,0,0);
    lv_obj_add_event_cb(App_btn_Back,App_btn_Back_Cb,LV_EVENT_ALL,parent);

    /* 添加返回按钮风格 */
    /* Init the style for the default state */
    static lv_style_t style;
    lv_style_init(&style);

    lv_style_set_radius(&style, 3);

    lv_style_set_bg_opa(&style, LV_OPA_100);
    lv_style_set_bg_color(&style, lv_palette_main(LV_PALETTE_YELLOW));
    lv_style_set_bg_grad_color(&style, lv_palette_darken(LV_PALETTE_YELLOW, 2));
    lv_style_set_bg_grad_dir(&style, LV_GRAD_DIR_VER);

    lv_style_set_border_opa(&style, LV_OPA_40);
    lv_style_set_border_width(&style, 2);
    lv_style_set_border_color(&style, lv_palette_main(LV_PALETTE_GREY));

    lv_style_set_shadow_width(&style, 8);
    lv_style_set_shadow_color(&style, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_shadow_ofs_y(&style, 8);

    lv_style_set_outline_opa(&style, LV_OPA_COVER);
    lv_style_set_outline_color(&style, lv_palette_main(LV_PALETTE_YELLOW));

    lv_style_set_text_color(&style, lv_color_white());
    lv_style_set_pad_all(&style, 10);

    /* Init the pressed style */
    static lv_style_t style_pr;
    lv_style_init(&style_pr);

    /*Add a large outline when pressed*/
    lv_style_set_outline_width(&style_pr, 30);
    lv_style_set_outline_opa(&style_pr, LV_OPA_TRANSP);

    lv_style_set_translate_y(&style_pr, 5);
    lv_style_set_shadow_ofs_y(&style_pr, 3);
    lv_style_set_bg_color(&style_pr, lv_palette_darken(LV_PALETTE_YELLOW, 2));
    lv_style_set_bg_grad_color(&style_pr, lv_palette_darken(LV_PALETTE_YELLOW, 4));

    /*Add a transition to the outline*/
    static lv_style_transition_dsc_t trans;
    static lv_style_prop_t props[] = {LV_STYLE_OUTLINE_WIDTH, LV_STYLE_OUTLINE_OPA, 0};
    lv_style_transition_dsc_init(&trans, props, lv_anim_path_linear, 300, 0, NULL);

    lv_style_set_transition(&style_pr, &trans);

    //lv_obj_remove_style_all(App_btn_Back);                          /*Remove the style coming from the theme*/
    lv_obj_add_style(App_btn_Back, &style, 0);
    lv_obj_add_style(App_btn_Back, &style_pr, LV_STATE_PRESSED);
    
    return parent;
}


/* 按钮矩阵数组 */
static const char *num_map[] = { "1", "2", "3", "\n",
                                 "4", "5", "6", "\n",
                                 "7", "8", "9", "\n",
                                 "ok", "0", "no", "" };


static lv_point_t points[] = {{0,0},{0,200}};       /* 线条坐标点数组 */

static const lv_font_t *font;                       /* 定义字体 */
static lv_obj_t *label_input;                       /* 输入框标签 */
static lv_obj_t *obj_input;                         /* 输入框背景 */
static char btnmap_buf[6];                          /* 按键输入值保存 */
static lv_obj_t *password_error;                    /* 密码错误消息框 */
/**
* @brief 按钮矩阵事件回调
* @param *e ：事件相关参数的集合，它包含了该事件的所有数据
* @return 无
*/
static void btnm_event_cb(lv_event_t* e)
{
    uint16_t id;
    lv_event_code_t code = lv_event_get_code(e);    /* 获取事件类型 */
    lv_obj_t *target = lv_event_get_target(e);      /* 获取触发源 */
    lv_obj_t* parent = lv_event_get_user_data(e);   /* 获取screen下的所有子对象 */
    lv_obj_draw_part_dsc_t* dsc = lv_event_get_draw_part_dsc(e);
    
    
    static uint8_t btnmap_index = 0;
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        id = lv_btnmatrix_get_selected_btn(target); /* 获取按键索引 */
        if(btnmap_index == 6 || !(strcmp(lv_btnmatrix_get_btn_text(target, id),"#")))
        {
            if(!strcmp(btnmap_buf,"123456"))
            {
                /* 这里只能clean掉lv_scr_act屏幕下的所有obj，不能del
                    因为会把lv_scr_act屏幕也del掉，会崩溃
                */
                lv_obj_clean(parent);
                /* 将标志置1，打开主界面 */
                lv_gui_app_screen();
            }else
            {
                /* 否则弹出密码错误的消息框 */
                password_error = lv_msgbox_create(NULL,"ERROR","The Passwoed is Wrong!!",NULL,true);
                lv_obj_center(password_error);
                memset(btnmap_buf,0,6);
                btnmap_index = 0;
            }
        }else
        {
            /* 将每次按键输入的数值追加到buf中 */
            strncat(btnmap_buf,lv_btnmatrix_get_btn_text(target, id),1);
            /* 更新输入框标签文本 */
            lv_label_set_text(label_input, btnmap_buf);
            /* 设置标签位置 */
            lv_obj_align_to(label_input, obj_input, LV_ALIGN_CENTER, 0, 0);
            /* 按键输入次数增加 */
            btnmap_index++;
        }
    }
}

/**
* @brief 密码输入界面
* @param 无
* @return 无
*/
static void lv_example_btnmatrix(lv_obj_t *parent)
{
    /* 根据屏幕宽度选择字体和图片缩放系数 */
    font = &myFont36;
    /* 图片显示 */
    lv_obj_t *img = lv_img_create(parent);                                                      /* 创建图片部件 */
    lv_img_set_src(img, png_load_path(user.bin));                                               /* 设置图片源 */
    lv_obj_align(img, LV_ALIGN_CENTER, -scr_act_width()/4, -scr_act_height()/7);                /* 设置位置 */
    lv_obj_set_style_img_recolor(img, lv_color_hex(0xf2f2f2),0);                                /* 设置重新着色 */
    lv_obj_set_style_img_recolor_opa(img,100,0);                                                /* 设置着色透明度 */

    /* 用户标签 */
    lv_obj_t *label_user = lv_label_create(parent);                                             /* 创建标签 */
    lv_label_set_text(label_user, "USER");                                                      /* 设置文本 */
    lv_obj_set_style_text_font(label_user, font, LV_PART_MAIN);                                 /* 设置字体 */
    lv_obj_set_style_text_color(label_user,lv_color_hex(0x7fc241),LV_PART_MAIN);                /* 字体为绿色 */
    lv_obj_set_style_text_align(label_user, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);                /* 设置文本居中 */
    lv_obj_align_to(label_user, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);                           /* 设置位置 */

    /* 输入框背景 */
    obj_input = lv_obj_create(lv_scr_act());                                                    /* 创建基础对象 */
    lv_obj_set_size(obj_input, scr_act_width()/4, scr_act_height()/12);                         /* 设置大小 */
    lv_obj_align_to(obj_input, label_user, LV_ALIGN_OUT_BOTTOM_MID, 0, scr_act_height()/20);    /* 设置位置 */
    lv_obj_set_style_bg_color(obj_input, lv_color_hex(0xcccccc),0);                             /* 设置背景颜色 */
    lv_obj_set_style_bg_opa(obj_input, 150,0);                                                  /* 设置透明度 */
    lv_obj_set_style_border_width(obj_input, 0 , 0);                                            /* 去除边框 */
    lv_obj_set_style_radius(obj_input, 20, 0);                                                  /* 设置圆角 */
    lv_obj_remove_style(obj_input, NULL, LV_PART_SCROLLBAR);                                    /* 移除滚动条 */

    /* 输入框文本标签 */
    label_input = lv_label_create(parent);                                                      /* 创建标签 */
    lv_label_set_text(label_input, "");                                                         /* 设置文本 */
    lv_obj_set_style_text_font(label_input, font, LV_PART_MAIN);                                /* 设置字体 */
    lv_obj_set_style_text_align(label_input, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);               /* 设置文本居中 */
    lv_obj_align_to(label_input, obj_input, LV_ALIGN_CENTER, 0, 0);                             /* 设置位置 */

    /* 分隔线 */
    lv_obj_t *line = lv_line_create(parent);                                                    /* 创建线条 */
    lv_line_set_points(line, points, 2);                                                        /* 设置线条坐标点 */
    lv_obj_align(line, LV_ALIGN_CENTER, 0, 0);                                                  /* 设置位置 */
    lv_obj_set_style_line_color(line, lv_color_hex(0xcdcdcd),0);                                /* 设置线条颜色 */

    /* 按钮矩阵（创建） */
    lv_obj_t *btnm = lv_btnmatrix_create(parent);                                               /* 创建按钮矩阵 */
    lv_obj_set_size(btnm, scr_act_width()* 2/5, scr_act_width()* 2/5);                          /* 设置大小 */
    lv_btnmatrix_set_map(btnm, num_map);                                                        /* 设置按钮 */
    lv_obj_align(btnm, LV_ALIGN_RIGHT_MID, -scr_act_width()/16, 0);                             /* 设置位置 */
    lv_obj_set_style_text_font(btnm, font, LV_PART_ITEMS);                                      /* 设置字体 */

    /* 按钮矩阵（优化界面） */
    lv_obj_set_style_border_width(btnm, 0, LV_PART_MAIN);                                       /* 去除主体边框 */
    lv_obj_set_style_bg_opa(btnm, 0, LV_PART_MAIN);                                             /* 设置主体背景透明度 */
    lv_obj_set_style_bg_opa(btnm, 0, LV_PART_ITEMS);                                            /* 设置按钮背景透明度 */
    lv_obj_set_style_shadow_width(btnm, 0, LV_PART_ITEMS);                                      /* 去除按钮阴影 */
    lv_obj_set_style_text_color(btnm, lv_color_hex(0xffe897),LV_PART_MAIN | LV_PART_ITEMS);     /* 设置按键按钮的颜色 */
    lv_obj_add_event_cb(btnm, btnm_event_cb, LV_EVENT_VALUE_CHANGED, parent);                   /* 设置按钮矩阵回调 */
}

/**
* @brief lvgl组件的心跳时基，单位1ms
* @param 无
* @return 无
*/
void bsp_RunPer1ms()
{
    lv_tick_inc(1);
}

