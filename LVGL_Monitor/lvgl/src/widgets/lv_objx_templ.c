/**
 * @file lv_templ.c
 *
 */

/**
 * TODO Remove these instructions
 * Search and replace: templ -> object short name with lower case(e.g. btn, label etc)
 *                    TEMPL -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 * You can remove the defined() clause from the #if statement below. This exists because
 * LV_USE_TEMPL is not in lv_conf.h or lv_conf_template.h by default.
 */

/*********************
 *      INCLUDES
 *********************/
//#include "lv_templ.h" /*TODO uncomment this*/

#include "lv_objx_templ.h"
#include "log.h"

#if defined(LV_USE_TEMPL) && LV_USE_TEMPL != 0

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_indicator_class


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_indicator_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_indicator_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_indicator_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void lv_btn_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_indicator_class = {
    .constructor_cb = lv_btn_constructor,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_indicator_t),
    .group_def = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .base_class = &lv_obj_class
};


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_panel_create(lv_obj_t* parent)
{
    LV_LOG_INFO("begin");
    LOG("cool control begian\r\n");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS,parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*======================
 * Add/remove functions
 *=====================*/

/*
 * New object specific "add" or "remove" functions come here
 */

/*=====================
 * Setter functions
 *====================*/

/*
 * New object specific "set" functions come here
 */

/*=====================
 * Getter functions
 *====================*/

/*
 * New object specific "get" functions come here
 */

/*=====================
 * Other functions
 *====================*/

/*
 * New object specific "other" functions come here
 */

/**********************
 *   STATIC FUNCTIONS
 **********************/
static void lv_btn_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    LV_TRACE_OBJ_CREATE("finished");
}


static void lv_indicator_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    lv_indicator_t * indicator = (lv_indicator_t *)obj;
    /*Free the widget specific data*/
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_SCROLL_ON_FOCUS);


    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_indicator_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_res_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    /*Add the widget specific event handling here*/
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
