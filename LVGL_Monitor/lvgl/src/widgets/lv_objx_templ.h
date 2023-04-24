/**
 * @file lv_templ.h
 *
 */

/**
 * TODO Remove these instructions
 * Search and replace: templ -> object short name with lower case(e.g. btn, label etc)
 *                    TEMPL -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 */

#ifndef LV_TEMPL_H
#define LV_TEMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lvgl.h"

#if LV_USE_TEMPL != 0

#include "../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of template*/
typedef struct {
    lv_obj_t obj;
    lv_color_t color;
    uint8_t bright;     /**< Current brightness of the LED (0..255)*/
} lv_indicator_t;


extern const lv_obj_class_t lv_indicator_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a templ object
 * @param parent    pointer to an object, it will be the parent of the new templ
 * @return          pointer to the created bar
 */
lv_obj_t * lv_panel_create(lv_obj_t* parent, const char* title, lv_coord_t height);
/*======================
 * Add/remove functions
 *=====================*/

/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_TEMPL*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TEMPL_H*/
