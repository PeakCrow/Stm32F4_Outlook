/**
 * @file lv_Frame.c
 *
 */

/**
 * TODO Remove these instructions
 * Search and replace: Frame -> object short name with lower case(e.g. btn, label etc)
 *                    Frame -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 * You can remove the defined() clause from the #if statement below. This exists because
 * LV_USE_Frame is not in lv_conf.h or lv_conf_Frameate.h by default.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_objx_frame.h" /*TODO uncomment this*/

#if LV_USE_Frame != 0

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_Frame_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_Frame_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_Frame_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_Frame_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_Frame_class = {
    .constructor_cb = lv_Frame_constructor,
    .destructor_cb = lv_Frame_destructor,
    .event_cb = lv_Frame_event,
    .width_def = LV_DPI_DEF,
    .height_def = LV_DPI_DEF,
    .instance_size = sizeof(lv_Frame_t),
    .group_def = LV_OBJ_CLASS_GROUP_DEF_INHERIT,
    .editable = LV_OBJ_CLASS_EDITABLE_INHERIT,
    .base_class = &lv_Frame_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_Frame_create(lv_obj_t * parent)
{

    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
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

static void lv_Frame_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_Frame_t * Frame = (lv_Frame_t *)obj;
    /*Initialize the widget's data*/

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_Frame_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    lv_Frame_t * Frame = (lv_Frame_t *)obj;
    /*Free the widget specific data*/
}

static void lv_Frame_event(const lv_obj_class_t * class_p, lv_event_t * e)
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
