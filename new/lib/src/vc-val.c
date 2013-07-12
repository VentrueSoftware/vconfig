/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 12-Jul-2013
 *    File: vc-val.c
 * 
 *    Definition for value types, and allocation of values.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vc-common.h"
#include "vc-val.h"


#define NEW(type) (type *)malloc(sizeof(type))
#define FREE(val) 


VCVAL *new_VCVAL(VCINT type, VCREF val) {
    VCVAL *newval = NEW(VCVAL);
    if (!newval) return NULL;

    return newval;
}

void del_VCVAL(VCVAL *val) {
    free(val);
}
