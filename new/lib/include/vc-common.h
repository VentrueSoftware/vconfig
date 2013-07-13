/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 12-Jul-2013
 *    File: vc-common.h
 *
 * Common C type definitions and constants.
 */

#include <stdint.h>

#ifndef VC_COMMON_H
#define VC_COMMON_H

/***********************************************************************
 * 
 * Type definitions
 * 
 **********************************************************************/

enum {
    TYPE_VCBOOL,
    TYPE_VCINT,
    TYPE_VCFLOAT,
    TYPE_VCCHAR,
    TYPE_VCSTR
};

typedef uint8_t     VCBOOL;
typedef int32_t     VCINT;
typedef double      VCFLOAT;
typedef char        VCCHAR;
typedef char*       VCSTR;  /* Not sure if this is a good idea yet */

/* Types not allowed in user values */
typedef void*       VCREF;  /* Generic reference to any type. */
typedef uint32_t    VCCNT;  /* Offset/count */
typedef uint8_t     VCBYTE; /* Byte */

#endif /* #ifndef VC_COMMON_H */
