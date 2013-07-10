/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 10-Jul-2013
 *    File: vcdirect.c
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

#include "vcdirect.h"

int vc_directive_add(vc_directives **dirlist, vc_directive *dir) {
    vc_directives *new;
    if (!dir || !dirlist) return 0;
    
    new = malloc(sizeof(vc_directives));
    bzero(new, sizeof(vc_directives));
    
    new->dir = dir;
    new->hash = hash_djb2((unsigned char *)dir->name);
    new->next = *dirlist;
    
    *dirlist = new;
    
    return 1;
}
int vc_directive_cleanup(vc_directives **dirlist) {
    vc_directives *ptr = *dirlist, *ptr2;
    if (!dirlist) return 0;
    
    while (ptr) {
        ptr2 = ptr->next;
        free(ptr);
        ptr = ptr2;
    }
    
    return 1;
}
