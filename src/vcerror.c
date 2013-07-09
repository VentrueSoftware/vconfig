/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 07-Jun-2013
 *    File: parse.c
 * 
 * Error handling/printing for VConfig.
 */

/**********************************************************************/
/**** Includes ********************************************************/
/**********************************************************************/
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vcerror.h"
#include "vcparse.h"

/**********************************************************************/
/**** Macro/Static Definitions ****************************************/
/**********************************************************************/

static const vc_error error_defs[] = {
    #define XX(type, flags, nargs, string) {flags, nargs, string},
    VC_ERROR_DEFS(XX)
    #undef XX
};

/**********************************************************************/
/**** Static Function Prototypes **************************************/
/**********************************************************************/
/* NONE */

/**********************************************************************/
/**** Function Definitions ********************************************/
/**********************************************************************/

/**********************************************************************/
/******** API Function Definitions ************************************/
/**********************************************************************/
void vc_print_error(vc_error_type err, struct vc_parser *parser, ...) {
    va_list args;
    va_start (args, parser);
    const vc_error *error = &(error_defs[err]);
    char buffer[256];

    
    bzero(buffer, 256);

    if (error->flags & O_FILE) {
        sprintf(buffer, "%s:%d: %s\n", parser->file, parser->line, error->msg);
    } else {
        sprintf(buffer, "%s\n", error->msg);
    }

    vprintf(buffer, args);
    va_end(args);
}

/**********************************************************************/
/******** Static Function Definitions *********************************/
/**********************************************************************/


#ifdef STANDALONE_VCERROR


#endif /* #ifdef STANDALONE */
