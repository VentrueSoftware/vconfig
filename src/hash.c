/* 
 * Project: VConfig
 *  Author: Kurt Sassenrath
 *    Date: 05-Jun-2013
 *    File: hash.h
 * 
 * Hash table implementation for configuration parser.  This relies 
 * on the excellent hash function djb2 by Dan Bernstein.
 */

/**********************************************************************/
/**** Includes ********************************************************/
/**********************************************************************/
#include "hash.h"
#include <stdio.h>

/**********************************************************************/
/**** Macro Definitions ***********************************************/
/**********************************************************************/
/* NONE */

/**********************************************************************/
/**** Static Function Prototypes **************************************/
/**********************************************************************/
uint32_t fasthash_insert_impl(fasthash_table *fh_table, char *key, void *entry);

fasthash_node *fasthash_node_construct(char *key, void *entry, fasthash_node *next);
fasthash_node *fasthash_node_destroy(fasthash_node *node, fasthash_destructor destruct);

/**********************************************************************/
/**** Function Definitions ********************************************/
/**********************************************************************/

/**********************************************************************/
/******** API Function Definitions ************************************/
/**********************************************************************/

/* FastHash Table Functions */
/** FastHash Table Initialization **/
fasthash_table *fasthash_init(uint32_t size, uint32_t opts, fasthash_destructor destruct) {
    fasthash_table *fh_table;
    
    if (!size) return 0;
    
    /* Allocate memory for the table structure */
    fh_table = (fasthash_table *)malloc(sizeof(fasthash_table));
    if (!fh_table) return 0;    /* Malloc error */
    
    bzero(fh_table, sizeof(fasthash_table));
    fh_table->size = size;
    fh_table->opts = opts;
    fh_table->destruct = destruct;
    
    /* Allocate memory for the actual entry table */
    fh_table->entries = malloc(sizeof(fasthash_node *) * size);
    if (!fh_table->entries) goto err1;
    
    bzero(fh_table->entries, sizeof(fasthash_node *) * size);
    
    return fh_table;

err1:
    free(fh_table);
    return 0;
}

/** FastHash Table Cleanup **/
fasthash_table *fasthash_cleanup(fasthash_table *fh_table) {
    uint32_t i;
    if (!fh_table) return 0;

    if (fh_table->entries) {
        if (fh_table->opts & FH_NO_INDEXING) {
            for (i = 0; i < fh_table->size; i++) {
                if (!fh_table->entries[i]) continue;
                fasthash_node_destroy(fh_table->entries[i], fh_table->destruct);
            }
        } else {
            index_node *temp = fh_table->index_list, *temp2;
            while (temp) {
                temp2 = temp->next;
                i = temp->index;
                fasthash_node_destroy(fh_table->entries[i], fh_table->destruct);
                free(temp);
                temp = temp2;
            }
        }
        free(fh_table->entries);
    }
    
    free(fh_table);
    
    return 0;
}

/** Insert **/
uint32_t fasthash_insert(fasthash_table *fh_table, char *key, void *entry) {
    return fasthash_insert_impl(fh_table, key, entry);
}
uint32_t fasthash_insertn(fasthash_table *fh_table, char *key, size_t length, void *entry) {
    uint32_t retval;
    char *key_sz;
    
    if (!fh_table) return 0;
    
    key_sz = malloc((length + 1) * sizeof(char));
    strncpy(key_sz, key, length);
    key_sz[length] = 0;
    
    retval = fasthash_insert_impl(fh_table, key_sz, entry);
    free(key_sz);
    return retval;
}

/** Force Insert **/
uint32_t fasthash_force_insert(fasthash_table *fh_table, char *key, void *entry) {
    if (!fh_table) return 0;
    uint32_t index = hash_djb2((unsigned char *)key) % fh_table->size;
    
    fh_table->entries[index] = entry;
    
    return index;
}

uint32_t fasthash_force_insertn(fasthash_table *fh_table, char *key, size_t length, void *entry) {
    if (!fh_table) return 0;
    uint32_t index = hashn_djb2((unsigned char *)key, length) % fh_table->size;
    
    fh_table->entries[index] = entry;
    
    return index;
}

/** Lookup **/
fasthash_node *fasthash_lookup(fasthash_table *fh_table, char *key) {
    if (!fh_table) return 0;
    uint32_t index = hash_djb2((unsigned char *)key) % fh_table->size;
    fasthash_node *node = fh_table->entries[index];
    
    while (node && (strcmp(key, node->key))) {
        node = node->next;
    }
    return node;
}
fasthash_node *fasthash_lookupn(fasthash_table *fh_table, char *key, size_t length) {
    if (!fh_table) return 0;
    uint32_t index = hashn_djb2((unsigned char *)key, length) % fh_table->size;
    fasthash_node *node = fh_table->entries[index];
    
    while (node && (length != strlen(node->key) || strncmp(key, node->key, length))) {
        node = node->next;
    }
    return node;
}


/* Hash Functions */
/** djb2 hash implementation **/
uint32_t hash_djb2(unsigned char *str) {
    uint32_t hash = 5381;       /* Initial hash */
    int c;                      /* Storage for char */
    
    while ((c = *str++)) {
        /* hash = hash * 33 + c */
        hash = ((hash << 5) + hash) + c;
    }
    
    return hash;
}

/** djb2 hash implementation, length-restricted **/
uint32_t hashn_djb2(unsigned char *str, size_t length) {
    uint32_t hash = 5381;       /* Initial hash */
    size_t len = 0;             /* Number of chars counted so far */
    int c;                      /* Storage for char */
    
    while ((c = *str++) && (len++ < length)) {
        /* hash = hash * 33 + c */
        hash = ((hash << 5) + hash) + c; 
    }
    
    return hash;
}

/**********************************************************************/
/******** Static Function Definitions *********************************/
/**********************************************************************/

fasthash_node *fasthash_node_construct(char *key, void *entry, fasthash_node *next) {
    fasthash_node *node = (fasthash_node *)malloc(sizeof(fasthash_node));
    node->key = strdup(key);
    node->data = entry;
    node->next = next;
    
    return node;
}

fasthash_node *fasthash_node_destroy(fasthash_node *node, fasthash_destructor destruct) {
    fasthash_node *temp;
    while (node) {
        temp = node->next;
        if (destruct) {
            destruct(node->data);
        }
        
        free(node->key);
        free(node);
        node = temp;
    }
    return 0;
}

uint32_t fasthash_insert_impl(fasthash_table *fh_table, char *key, void *entry) {
    if (!fh_table) return 0;    
    uint32_t index = hash_djb2((unsigned char *)key) % fh_table->size;
    fasthash_node *node = fh_table->entries[index];

    if (node) {
        /* If we aren't allowing collisions, return out of range */
        if (fh_table->opts & FH_NO_COLLISIONS) {
            return fh_table->size + 1;
        }
    } else if (!(fh_table->opts & FH_NO_INDEXING)) {
        /* Build index node if we are indexing */
        index_node *in = (index_node *)malloc(sizeof(index_node));
        in->index = index;
        in->next = fh_table->index_list;
        fh_table->index_list = in;
    }
    
    node = fasthash_node_construct(key, entry, node);
    //printf("Node \"%s\" = %p\n", key, entry);
    fh_table->entries[index] = node;
    
    return index;   /* We don't need to add an additional index */    
}
