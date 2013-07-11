/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 10-Jun-2013
 *    File: vctype.h
 * 
 * Option and section abstraction for VConfig.  Instead of defining this
 * within the parser or the main VConfig code, calls were placed in here.
 */

#ifndef __VCTYPE_H
#define __VCTYPE_H

/**********************************************************************/
/**** Begin Includes **************************************************/
/**********************************************************************/
#include "hash.h"
#include "vcdirect.h"

/**********************************************************************/
/**** Begin Type Definitions ******************************************/
/**********************************************************************/

#define VC_OPT_TYPES(XX)            \
    XX(ERROR)                       \
    XX(BOOLEAN)                     \
    XX(INTEGER)                     \
    XX(FLOAT)                       \
    XX(STRING)                      \
    XX(SECTION)

/* VConfig Option Type Enum */
typedef enum {
    #define XX(name) VC_##name,
    VC_OPT_TYPES(XX)
    #undef XX
} vc_type;

/* Container for VConfig Options */
typedef struct vc_opt {
    vc_type type;
    void *value;
} vc_opt;

/* Container for list-style VConfig options, compatible with vc_opt */
typedef struct vc_list {
    vc_type type;
    void *value;
    struct vc_list *next;
} vc_list;

/* VConfig Section type definition */
typedef struct vc_sect {
    /* TBD - Do I need anything else in here? */
    fasthash_table *ht;      /* Hash table to store vc_opt values */
} vc_sect;
typedef vc_sect vconfig;

typedef int (*vc_dirfunc)(vc_sect *, vc_list *);

/* Contains the directive name, format string, and handler. */
typedef struct vc_directive {
    char *name;         /* Directive name */
    int flags;          /* Directive flags */
    char format[16];    /* Format string for parameters */
    vc_dirfunc func;    /* Function handler */
} vc_directive;

typedef struct vc_params {
    char *file;                 /* Name of file to open */
    vc_directive *directives;   /* Directives list to use */
} vc_params;

struct vc_token;
/**********************************************************************/
/**** Begin Definitions/Static Declarations ***************************/
/**********************************************************************/
/* NONE */

/**********************************************************************/
/**** Begin Function Prototypes ***************************************/
/**********************************************************************/
vc_sect *vc_root_sect(void);
vc_sect *vc_sect_create(void);
void vc_sect_destroy(vc_sect *sect);


/* Add a new VConfig option value within a VConfig section */
vc_opt *vc_addopt(vc_sect *sect, char *name, struct vc_token *token);
vc_opt *vc_addoptn(vc_sect *sect, char *name, size_t length, struct vc_token *token);

/* Get VConfig option, within the container. */
vc_opt *vc_getopt(vc_sect *sect, char *optpath);

/* Get VConfig option value.  You must know the type ahead of time for
 * this one. */
void *vc_getval(vc_sect *sect, char *optpath);

vc_opt *vc_opt_create(struct vc_token *token);
void vc_opt_destroy(void *opt);


#endif /* #ifndef __VCTYPE_H */
