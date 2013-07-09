/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 05-Jun-2013
 *    File: hash.h
 * 
 * Hash table implementation for configuration parser.  This relies 
 * on the excellent hash function djb2 by Dan Bernstein.
 */
 
#ifndef __HASH_H
#define __HASH_H

/**********************************************************************/
/**** Begin Includes **************************************************/
/**********************************************************************/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define FH_NO_COLLISIONS 1
#define FH_NO_INDEXING 2

/**********************************************************************/
/**** Begin Type Definitions ******************************************/
/**********************************************************************/

/* Basic linked list structure for keeping track of indices we've used
 * in the FastHash table */
typedef struct index_node {
    uint32_t index;             /* FastHash table index to data */
    struct index_node *next;    /* Next index */
} index_node;

/* This handle is used when destroying the FastHash table; it is called
 * once for every node in the table. */
typedef void (*fasthash_destructor)(void *data);

/* The FastHash table node provides basic linked-list functionality for
 * collision handling, and stores a copy of the key so comparisons can
 * be made during searches */
typedef struct fasthash_node {
    char *key;                  /* Key of node */
    void *data;                 /* Data reference */
    struct fasthash_node *next; /* Next fasthash node (for collisions) */
} fasthash_node;

/* FastHash table definition.  Asside from options and size parameters,
 * the table stores nodes in a malloc'd array, and keeps a linked-list
 * of indices used in the table for table destruction. */
typedef struct fasthash_table {
    uint32_t opts;                  /* FastHash table options */
    uint32_t size;                  /* Size of hash table */
    
    fasthash_node **entries;        /* Entries of hash table */
    index_node *index_list;         /* List of entries in the table */
    
    fasthash_destructor destruct;   /* Node data destructor handle */
} fasthash_table;
/**********************************************************************/
/**** Begin Definitions/Static Declarations ***************************/
/**********************************************************************/
/* NONE */

/**********************************************************************/
/**** Begin Function Prototypes ***************************************/
/**********************************************************************/

/* FastHash Table Functions */
/** Create/Destroy **/
fasthash_table *fasthash_init(uint32_t size, uint32_t opts, fasthash_destructor destruct);
fasthash_table *fasthash_cleanup(fasthash_table *);

/** Insert **/
uint32_t fasthash_insert(fasthash_table *fh_table, char *key, void *entry);
uint32_t fasthash_insertn(fasthash_table *fh_table, char *key, size_t length, void *entry);

/** Force Insert **/
uint32_t fasthash_force_insert(fasthash_table *fh_table, char *key, void *entry);
uint32_t fasthash_force_insertn(fasthash_table *fh_table, char *key, size_t length, void *entry);

/** Lookup **/
fasthash_node *fasthash_lookup(fasthash_table *fh_table, char *key);
fasthash_node *fasthash_lookupn(fasthash_table *fh_table, char *key, size_t length);

/* Hash Functions */
/** djb2 hash implementation **/
uint32_t hash_djb2(unsigned char *str);
uint32_t hashn_djb2(unsigned char *str, size_t length);

#endif /* #ifndef HASH_H */
