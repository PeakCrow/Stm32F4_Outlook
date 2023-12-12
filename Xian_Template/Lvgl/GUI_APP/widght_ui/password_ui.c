#include "password_ui.h"

#define scr_act_height() lv_obj_get_height(lv_scr_act())
#define scr_act_width()  lv_obj_get_width(lv_scr_act())

static lv_obj_t *password_ui;

/* 按钮矩阵数组 */
static const char *num_map[] = { "1", "2", "3", "\n",
                                 "4", "5", "6", "\n",
                                 "7", "8", "9", "\n",
                                 "#", "0", "%", "" };


static lv_point_t points[] = {{0,0},{0,200}};       /* 线条坐标点数组 */

static const lv_font_t *font;                       /* 定义字体 */
static lv_obj_t *label_input;                       /* 输入框标签 */
static lv_obj_t *obj_input;                         /* 输入框背景 */
static char btnmap_buf[6];                          /* 按键输入值保存 */
static lv_obj_t *password_error;                    /* 密码错误消息框 */

static void lv_gui_app_screen(void);

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


/**
* @brief 按钮矩阵事件回调
* @param *e ：事件相关参数的集合，它包含了该事件的所有数据
* @return 无
*/
static void btnm_event_cb(lv_event_t* e)
{
    uint16_t id;
    lv_event_code_t code = lv_event_get_code(e); /* 获取事件类型 */
    lv_obj_t *target = lv_event_get_target(e); /* 获取触发源 */
    static uint8_t btnmap_index = 0;
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        id = lv_btnmatrix_get_selected_btn(target); /* 获取按键索引 */
        if(btnmap_index == 6 || !(strcmp(lv_btnmatrix_get_btn_text(target, id),"#")))
        {
            if(!strcmp(btnmap_buf,"123456"))
            {
                lv_obj_clean(password_ui);
                /* 将标志置1，打开主界面 */
                lv_gui_app_screen();
            }else
            {
                /* 否则弹出密码错误的消息框 */
                password_error = lv_msgbox_create(NULL,"ERROR","The Password is Wrong!!",NULL,true);
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
lv_obj_t* lv_example_btnmatrix(lv_obj_t *parent)
{
    /* 根据屏幕宽度选择字体和图片缩放系数 */
    font = &lv_font_montserrat_24;
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
    lv_obj_add_event_cb(btnm, btnm_event_cb, LV_EVENT_VALUE_CHANGED, NULL);                     /* 设置按钮矩阵回调 */
    
    return parent;
}
