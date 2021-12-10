#ifndef HASH_H
#define HASH_H

#include "list.h"
#include "tree.h"

#define HASH_SIZE 211

typedef struct hash_s {
    list_t* table[HASH_SIZE];   /* hash table */
    struct hash_s* next;        /* stackable */
    list_t* scope_owner;        /* subprogram that owns scope */
}
hash_t;

/* constructor/destructor */
hash_t* make_hash();
void free_hash();
void print_hash( hash_t* p );

/* search+insert over hash table */
list_t* search_hash( hash_t* scope, char* name );   /* search local scope */
list_t* insert_hash( hash_t* scope, char* name );   /* insert into top scope */

list_t* get_objects( hash_t* scope );   /* get all objects local to scope */

/* stackable */
hash_t* push_scope( hash_t* top );//, list_t* owner );
hash_t* pop_scope( hash_t* top );
list_t* search_scopes( hash_t* scope, char* name ); /* search all scopes, starting from top scope */
void print_scopes( hash_t* top_scope );


#endif
