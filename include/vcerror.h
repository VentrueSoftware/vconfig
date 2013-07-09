/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 07-Jun-2013
 *    File: parse.h
 * 
 * Error handling/printing for VConfig.
 */

#ifndef __VCERROR_H
#define __VCERROR_H

/**********************************************************************/
/**** Begin Includes **************************************************/
/**********************************************************************/
#include <string.h>

/**********************************************************************/
/**** Begin Type Definitions ******************************************/
/**********************************************************************/

#define O_0ARG 0x01
#define O_1ARG 0x02
#define O_2ARG 0x04
#define O_3ARG 0x08
#define O_FILE 0x10

#define ARG_MASK 0x0F

struct vc_parser;

/* VConfig error types */
#define VC_ERROR_DEFS(XX)                                                                               \
    XX(SUCCESS,         0,      0, "No error encountered.")                                             \
    XX(FILE,            0,      1, "File Error: Unable to open config file '%s'")                       \
    XX(UNEXPECTED_EOF,  O_FILE, 0, "Syntax error: Unexpected end of file.")                             \
    XX(UNEXPECTED,      O_FILE, 3, "Syntax error: Unexpected token: %s (value: %.*s) ")                 \
    XX(EXPECTED,        O_FILE, 2, "Syntax error: Expected %s instead of %s")                           \
    XX(INVALID_TOKEN,   O_FILE, 1, "Syntax error: Invalid token '%.*s'")                                \
    XX(SECT_MISMATCH,   O_FILE, 4, "Syntax error: Expected end of section for '%.*s', not '%.*s'.")     \
    XX(DEPTH_UNDERFLOW, O_FILE, 0, "Syntax error: End of section found when already at root section.")  \
    XX(DEPTH_OVERFLOW,  O_FILE, 1, "Syntax error: Exceeded maximum section depth %d. Use fewer subsections.")    

typedef enum {
    #define XX(type, flags, nargs, string) VC_ERROR_##type,
    VC_ERROR_DEFS(XX)
    #undef XX
} vc_error_type;

typedef struct vc_error {
    int flags;
    int nargs;
    char *msg;
} vc_error;

/**********************************************************************/
/**** Begin Definitions/Static Declarations ***************************/
/**********************************************************************/
void vc_print_error(vc_error_type err, struct vc_parser *parser, ...);

/**********************************************************************/
/**** Begin Function Prototypes ***************************************/
/**********************************************************************/
/* NONE */

#endif /* #ifndef __VCERROR_H */
