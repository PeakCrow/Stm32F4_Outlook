#include "password_ui.h"

#define scr_act_height() lv_obj_get_height(lv_scr_act())
#define scr_act_width()  lv_obj_get_width(lv_scr_act())

static lv_obj_t *password_ui;

/* ��ť�������� */
static const char *num_map[] = { "1", "2", "3", "\n",
                                 "4", "5", "6", "\n",
                                 "7", "8", "9", "\n",
                                 "#", "0", "%", "" };


static lv_point_t points[] = {{0,0},{0,200}};       /* ������������� */

static const lv_font_t *font;                       /* �������� */
static lv_obj_t *label_input;                       /* ������ǩ */
static lv_obj_t *obj_input;                         /* ����򱳾� */
static char btnmap_buf[6];                          /* ��������ֵ���� */
static lv_obj_t *password_error;                    /* ���������Ϣ�� */

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
    /* �����ٶ��� */
    /* ��ʱ���Ǳ��̷��������棬�Դ��������������� */
    lv_example_Monitor_Speed_Meter();
}


/**
* @brief ��ť�����¼��ص�
* @param *e ���¼���ز����ļ��ϣ��������˸��¼�����������
* @return ��
*/
static void btnm_event_cb(lv_event_t* e)
{
    uint16_t id;
    lv_event_code_t code = lv_event_get_code(e); /* ��ȡ�¼����� */
    lv_obj_t *target = lv_event_get_target(e); /* ��ȡ����Դ */
    static uint8_t btnmap_index = 0;
    if (code == LV_EVENT_VALUE_CHANGED)
    {
        id = lv_btnmatrix_get_selected_btn(target); /* ��ȡ�������� */
        if(btnmap_index == 6 || !(strcmp(lv_btnmatrix_get_btn_text(target, id),"#")))
        {
            if(!strcmp(btnmap_buf,"123456"))
            {
                lv_obj_clean(password_ui);
                /* ����־��1���������� */
                lv_gui_app_screen();
            }else
            {
                /* ���򵯳�����������Ϣ�� */
                password_error = lv_msgbox_create(NULL,"ERROR","The Password is Wrong!!",NULL,true);
                lv_obj_center(password_error);
                memset(btnmap_buf,0,6);
                btnmap_index = 0;
            }
        }else
        {
            /* ��ÿ�ΰ����������ֵ׷�ӵ�buf�� */
            strncat(btnmap_buf,lv_btnmatrix_get_btn_text(target, id),1);
            /* ����������ǩ�ı� */
            lv_label_set_text(label_input, btnmap_buf);
            /* ���ñ�ǩλ�� */
            lv_obj_align_to(label_input, obj_input, LV_ALIGN_CENTER, 0, 0);
            /* ��������������� */
            btnmap_index++;
        }
    }
}

/**
* @brief �����������
* @param ��
* @return ��
*/
lv_obj_t* lv_example_btnmatrix(lv_obj_t *parent)
{
    /* ������Ļ���ѡ�������ͼƬ����ϵ�� */
    font = &lv_font_montserrat_24;
    /* ͼƬ��ʾ */
    lv_obj_t *img = lv_img_create(parent);                                                      /* ����ͼƬ���� */
    lv_img_set_src(img, png_load_path(user.bin));                                               /* ����ͼƬԴ */
    lv_obj_align(img, LV_ALIGN_CENTER, -scr_act_width()/4, -scr_act_height()/7);                /* ����λ�� */
    lv_obj_set_style_img_recolor(img, lv_color_hex(0xf2f2f2),0);                                /* ����������ɫ */
    lv_obj_set_style_img_recolor_opa(img,100,0);                                                /* ������ɫ͸���� */

    /* �û���ǩ */
    lv_obj_t *label_user = lv_label_create(parent);                                             /* ������ǩ */
    lv_label_set_text(label_user, "USER");                                                      /* �����ı� */
    lv_obj_set_style_text_font(label_user, font, LV_PART_MAIN);                                 /* �������� */
    lv_obj_set_style_text_align(label_user, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);                /* �����ı����� */
    lv_obj_align_to(label_user, img, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);                           /* ����λ�� */

    /* ����򱳾� */
    obj_input = lv_obj_create(lv_scr_act());                                                    /* ������������ */
    lv_obj_set_size(obj_input, scr_act_width()/4, scr_act_height()/12);                         /* ���ô�С */
    lv_obj_align_to(obj_input, label_user, LV_ALIGN_OUT_BOTTOM_MID, 0, scr_act_height()/20);    /* ����λ�� */
    lv_obj_set_style_bg_color(obj_input, lv_color_hex(0xcccccc),0);                             /* ���ñ�����ɫ */
    lv_obj_set_style_bg_opa(obj_input, 150,0);                                                  /* ����͸���� */
    lv_obj_set_style_border_width(obj_input, 0 , 0);                                            /* ȥ���߿� */
    lv_obj_set_style_radius(obj_input, 20, 0);                                                  /* ����Բ�� */
    lv_obj_remove_style(obj_input, NULL, LV_PART_SCROLLBAR);                                    /* �Ƴ������� */

    /* ������ı���ǩ */
    label_input = lv_label_create(parent);                                                      /* ������ǩ */
    lv_label_set_text(label_input, "");                                                         /* �����ı� */
    lv_obj_set_style_text_font(label_input, font, LV_PART_MAIN);                                /* �������� */
    lv_obj_set_style_text_align(label_input, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);               /* �����ı����� */
    lv_obj_align_to(label_input, obj_input, LV_ALIGN_CENTER, 0, 0);                             /* ����λ�� */

    /* �ָ��� */
    lv_obj_t *line = lv_line_create(parent);                                                    /* �������� */
    lv_line_set_points(line, points, 2);                                                        /* ������������� */
    lv_obj_align(line, LV_ALIGN_CENTER, 0, 0);                                                  /* ����λ�� */
    lv_obj_set_style_line_color(line, lv_color_hex(0xcdcdcd),0);                                /* ����������ɫ */

    /* ��ť���󣨴����� */
    lv_obj_t *btnm = lv_btnmatrix_create(parent);                                               /* ������ť���� */
    lv_obj_set_size(btnm, scr_act_width()* 2/5, scr_act_width()* 2/5);                          /* ���ô�С */
    lv_btnmatrix_set_map(btnm, num_map);                                                        /* ���ð�ť */
    lv_obj_align(btnm, LV_ALIGN_RIGHT_MID, -scr_act_width()/16, 0);                             /* ����λ�� */
    lv_obj_set_style_text_font(btnm, font, LV_PART_ITEMS);                                      /* �������� */

    /* ��ť�����Ż����棩 */
    lv_obj_set_style_border_width(btnm, 0, LV_PART_MAIN);                                       /* ȥ������߿� */
    lv_obj_set_style_bg_opa(btnm, 0, LV_PART_MAIN);                                             /* �������屳��͸���� */
    lv_obj_set_style_bg_opa(btnm, 0, LV_PART_ITEMS);                                            /* ���ð�ť����͸���� */
    lv_obj_set_style_shadow_width(btnm, 0, LV_PART_ITEMS);                                      /* ȥ����ť��Ӱ */
    lv_obj_add_event_cb(btnm, btnm_event_cb, LV_EVENT_VALUE_CHANGED, NULL);                     /* ���ð�ť����ص� */
    
    return parent;
}
