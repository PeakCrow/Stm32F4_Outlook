/**
 * @file lv_Frame.h
 *
 */

/**
 * TODO Remove these instructions
 * Search and replace: Frame -> object short name with lower case(e.g. btn, label etc)
 *                    Frame -> object short name with upper case (e.g. BTN, LABEL etc.)
 *
 */

#ifndef LV_Frame_H
#define LV_Frame_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"
#include "lv_conf.h"

#if LV_USE_Frame != 0

#include "../core/lv_obj.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of Frameate*/
typedef struct {
    //lv_ANCESTOR_t ancestor; /*The ancestor widget, e.g. lv_slider_t slider*/
    /*New data for this type*/
} lv_Frame_t;

extern const lv_obj_class_t lv_Frame_class;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a Frame object
 * @param parent    pointer to an object, it will be the parent of the new Frame
 * @return          pointer to the created bar
 */
lv_obj_t * lv_Frame_create(lv_obj_t * parent);

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

#endif /*LV_USE_Frame*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_Frame_H*/
