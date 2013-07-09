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

/**********************************************************************/
/**** Macro Definitions ***********************************************/
/**********************************************************************/

/**********************************************************************/
/**** Static Function Prototypes **************************************/
/**********************************************************************/
/* Create/Destroy VConfig option containers */
vc_opt *vc_opt_create(vc_opt_type type, void *value);
void vc_opt_destroy(void *opt);
/**********************************************************************/
/**** Function Definitions ********************************************/
/**********************************************************************/

/**********************************************************************/
/******** API Function Definitions ************************************/
/**********************************************************************/

vc_sect *vc_root_sect(void) {
    return vc_sect_create();
}

vc_opt *vc_opt_create(vc_opt_type type, void *value) {
    vc_opt *opt = (vc_opt *)malloc(sizeof(vc_opt));
    if (!opt) return 0;
    
    opt->type = type;
    if (opt->type == VC_OPT_SECTION) {
        opt->value = vc_sect_create();
    } else {
        opt->value = value;
    }
    
    return opt;
}

void vc_opt_destroy(void *data) {
    vc_opt *opt = (vc_opt *)data;
    if (!data) return;
    
    switch(opt->type) {
        case VC_OPT_BOOLEAN:
        case VC_OPT_INTEGER:
            free((int *)opt->value);
        break;
        case VC_OPT_STRING:
            free((char *)opt->value);
        break;
        case VC_OPT_SECTION:
            vc_sect_destroy((vc_sect *)opt->value);
        break;
        default:
        break;
    }
    
    free(opt);
}

/* Add a new VConfig option value within a VConfig section */
vc_opt *vc_addopt(vc_sect *sect, char *name, vc_opt_type type, void *value) {
    vc_opt *opt = vc_opt_create(type, value);
    if (!opt) return 0;
    
    fasthash_insert(sect->ht, name, opt);
    return opt;
}

/* Add a new VConfig option value within a VConfig section */
vc_opt *vc_addoptn(vc_sect *sect, char *name, size_t length, vc_opt_type type, void *value) {
    vc_opt *opt = vc_opt_create(type, value);
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
    if (!node) return NULL;
    opt = node->data;
    
    if (*ptr && opt->type != VC_OPT_SECTION) {
        return NULL;
    } else if (*ptr && opt->type == VC_OPT_SECTION) {
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

#ifdef STANDALONE_VCTYPE

#include <stdio.h>

int main(int argc, char **argv) {
    char *numstrings[9];    
    vc_sect *root = vc_sect_create();
    vc_opt *s1opt = vc_addopt(root, "s1", VC_OPT_SECTION, 0);
    vc_opt *opt;
    vc_sect *s1 = (vc_sect *)vc_getval(root, "s1");
    char buffer[256];
    int i;
    
    numstrings[1] = strdup("one");
    numstrings[2] = strdup("two");
    numstrings[3] = strdup("three");
    numstrings[4] = strdup("four");
    numstrings[5] = strdup("five");
    numstrings[6] = strdup("six");
    numstrings[7] = strdup("seven");
    //numstrings[8] = strdup("eight");
    
    
    for (i = 1; i < argc; i++) {
        opt = vc_addopt(s1, argv[i], VC_OPT_STRING, numstrings[i]);
        if (!opt) printf("ERROR for argv[%d]\n", i);
    }
    
    for (i = 1; i < argc; i++) {
        char *v = (char *)vc_getval(s1, argv[i]);
        printf("%s = %s\n", argv[i], v);
    }
    
    for (i = 1; i < argc; i++) {
        char *v;
        bzero(buffer, 256);
        strcpy(buffer, "s1.");
        strcat(buffer, argv[i]);
        v = (char *)vc_getval(s1, argv[i]);
        printf("%s = %s\n", buffer, v ? v : "(nil)");
    }
    
    vc_sect_destroy(root);
    
    return 0;
}

#endif /* #ifdef STANDALONE */
