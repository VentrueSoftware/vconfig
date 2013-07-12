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

#define TYPE_VCINT 0x01
#define TYPE_VCFLOAT 0x02
#define TYPE_VCCHAR 0x04
#define TYPE_VCSTR  0x08

typedef int32_t     VCINT;
typedef double      VCFLOAT;
typedef char        VCCHAR;
typedef char*       VCSTR;  /* Not sure if this is a good idea yet */

/* Types not allowed in user values */
typedef void*       VCREF;  /* Generic reference to any type. */
typedef uint32_t    VCCNT;  /* Offset/count */
typedef uint8_t     VCBYTE; /* Byte */

#endif /* #ifndef VC_COMMON_H */
