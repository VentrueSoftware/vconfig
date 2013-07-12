/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 12-Jul-2013
 *    File: vc-val.h
 * 
 *    Definition for value.
 */

#ifndef VC_VAL_H
#define VC_VAL_H


/**********************************************************************
 * 
 * Type definition for VCVAL
 * 
 **********************************************************************/
 
struct vcfg_value;
typedef struct vcfg_value VCVAL;

struct vcfg_value {
    VCINT flags;    /* Determines type */
    union {
        VCINT   _int;       /* Integer value */
        VCFLOAT _float;     /* Floating point */
        VCCHAR  _char;      /* Character */
        VCSTR   _str;       /* String */
        
        #ifdef VCGRP_IMPL        
        VCGRP  _grp;        /* Group of values. Not defined yet. */
        #endif /* #ifdef GRP_IMPL */
    } data;
};

/***********************************************************************
 * 
 * Macros for accessing VCVAL values
 * 
 **********************************************************************/
 
#define VAL_INT(v)      ((v)->data._int)
#define VAL_FLOAT(v)    ((v)->data._float)
#define VAL_CHAR(v)     ((v)->data._char)
#define VAL_STR(v)      ((v)->data._str)

#ifdef VCGRP_IMPL
#define VAL_GRP(v)      ((v)->data._grp)
#endif /* #ifdef VCGRP_IMPL */

/***********************************************************************
 * 
 * Allocating a new VCVAL.
 * 
 **********************************************************************/

extern VCVAL *new_VCVAL(VCINT type, VCREF val);
extern void   del_VCVAL(VCVAL *val);

#endif /* #ifndef VC_VAL_H */
