/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 10-Jul-2013
 *    File: vcdirect.h
 * 
 * Directive creation and handling.  Think of directives as user-defined
 * functions within a configuration file, yet the definition is in C.
 * 
 * A splay tree or a trie indexed by hashing the string should provide
 * quick access and not slow down configuration parsing. For now, I just
 * use a list.
 * 
 * Arguments are passed into a directive function as a vc_list; while this
 * is a bit uncomfortable, rest assured, vconfig typechecks for you, so
 * all you have to do is pull the arguments out of the list into variables
 * that you wish to use.
 */

#ifndef __VCDIRECT_H
#define __VCDIRECT_H

#include "vctype.h"

typedef int (*vc_dirfunc)(vc_sect *, vc_list *);

/* Contains the directive name, format string, and handler. */
typedef struct vc_directive {
    char *name;         /* Directive name */
    vc_dirfunc func;    /* function handler */
    vc_type format[];    /* Format of directive arguments */
} vc_directive;

/* Linked list container for directives.  This will make changing the
 * data structure of directives easier */
typedef struct vc_directives {
    uint32_t hash;              /* Hash value of directive name */
    vc_directive *dir;          /* Directive */
    struct vc_directives *next; /* Next directive */
} vc_directives;

int vc_directive_add(vc_directives **dirlist, vc_directive *dir);
int vc_directive_cleanup(vc_directives **dirlist);
#endif /* #ifndef __VCDIRECT_H */
