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

#endif /* #ifndef __VCDIRECT_H */
