/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 11-Jun-2013
 *    File: vctype.c
 * 
 * Option and section abstraction for VConfig.  Instead of defining this
 * within the parser or the main VConfig code, calls were placed in here.
 */
/**********************************************************************/
/**** Includes ********************************************************/
/**********************************************************************/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "vctype.h"
#include "vcparse.h"

/**********************************************************************/
/**** Macro Definitions ***********************************************/
/**********************************************************************/

/**********************************************************************/
/**** Static Function Prototypes **************************************/
/**********************************************************************/
/* Create/Destroy VConfig option containers */
/**********************************************************************/
/**** Function Definitions ********************************************/
/**********************************************************************/

/**********************************************************************/
/******** API Function Definitions ************************************/
/**********************************************************************/

vc_sect *vc_root_sect(void) {
    return vc_sect_create();
}

vc_opt *vc_opt_create(vc_token *token) {
    vc_opt *opt = (vc_opt *)malloc(sizeof(vc_opt));
    opt->value = 0;
    if (!opt) return 0;
    
    switch (token->type) {
        case VC_TOKEN_SECT_BEGIN:
            opt->type = VC_SECTION;
            opt->value = vc_sect_create();
        break;
        case VC_TOKEN_BOOLEAN: {
            int *v = malloc(sizeof(int));
            *v = token->length;
            opt->value = v;
        } break;
        case VC_TOKEN_INTEGER: case VC_TOKEN_FLOAT: {
            char str[32];
            strncpy(str, token->position, token->length);
            str[token->length] = '\0';
            if (token->type == VC_TOKEN_FLOAT) {
                double *v = malloc(sizeof(double));
                *v = atof(str);
                opt->type = VC_FLOAT;
                opt->value = v; 
            } else {
                int *v = malloc(sizeof(int));
                *v = atoi(str);
                opt->type = VC_INTEGER;
                opt->value = v;
            }
        } break;
        case VC_TOKEN_STRING: {
            char *v = (char *)malloc(sizeof(char) * (token->length + 1));
            strncpy(v, token->position, token->length);
            v[token->length] = '\0';
            opt->type = VC_STRING;
            opt->value = v;
        } break;
        default:
        break;
    }
    
    if (opt->value) return opt;
    
    free(opt); 
    return 0;
}

void vc_opt_destroy(void *data) {
    vc_opt *opt = (vc_opt *)data;
    if (!data) return;
    
    switch(opt->type) {
        case VC_BOOLEAN:
        case VC_INTEGER:
            free((int *)opt->value);
        break;
        case VC_STRING:
            free((char *)opt->value);
        break;
        case VC_SECTION:
            vc_sect_destroy((vc_sect *)opt->value);
        break;
        default:
        break;
    }
    
    free(opt);
}

/* Add a new VConfig option value within a VConfig section */
vc_opt *vc_addopt(vc_sect *sect, char *name, vc_token *token) {
    vc_opt *opt = vc_opt_create(token);
    if (!opt) return 0;
    
    fasthash_insert(sect->ht, name, opt);
    return opt;
}

/* Add a new VConfig option value within a VConfig section */
vc_opt *vc_addoptn(vc_sect *sect, char *name, size_t length, vc_token *token) {
    vc_opt *opt = vc_opt_create(token);
    printf("opt: %p\n", opt);
    if (!opt) return 0;
    
    fasthash_insertn(sect->ht, name, length, opt);
    return opt;
}

/* Get VConfig option, within the container. */
vc_opt *vc_getopt(vc_sect *sect, char *optpath) {
    char *ptr = optpath;
    fasthash_node *node;
    vc_opt *opt;
    
    while (*ptr && *ptr != '.') ptr++;
    node = fasthash_lookupn(sect->ht, optpath, ptr - optpath);
    if (!node) return NULL; /* Optpath not found */
    opt = node->data;
    
    if (*ptr && opt->type != VC_SECTION) {
        /* If *ptr != 0 and we aren't at a section, then we don't return
         * anything as our query won't be matched. */
        return NULL;
    } else if (*ptr && opt->type == VC_SECTION) {
        /* Recurse into the next section. */
        return vc_getopt((vc_sect *)opt->value, ptr + 1);
    } else {
        return opt;
    }
    return NULL;
}

/* Get VConfig option value.  You must know the type ahead of time for
 * this one. */
void *vc_getval(vc_sect *sect, char *optpath) {
    vc_opt *opt = vc_getopt(sect, optpath);
    if (opt) return opt->value;
    return NULL;
}

/**********************************************************************/
/******** Static Function Definitions *********************************/
/**********************************************************************/

vc_sect *vc_sect_create(void) {
    vc_sect *sect = (vc_sect *)malloc(sizeof(vc_sect));
    if (!sect) return 0;
    
    sect->ht = fasthash_init(256, 0, vc_opt_destroy);
    
    return sect;
}

void vc_sect_destroy(vc_sect *sect) {
    if (!sect) return;
    
    fasthash_cleanup(sect->ht);
    free(sect);
}
