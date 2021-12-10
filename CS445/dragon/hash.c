#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hash.h"

int hashpjw( char* );

/* Weinberger's magic hash: Chapter 7 (Dragon1) */
int hashpjw( char *s )
{
    char* p;
    unsigned h = 0, g;

    for( p=s; *p!='\0'; p++ )
    {
        h = (h << 4) + (*p);
        if( (g = h & 0xf0000000) )
        {
            h = h ^ (g >> 24);
            h = h ^ g;
        }
    }
    return h % HASH_SIZE;
}

/* constructor/destructor */
hash_t* make_hash()
{
    int i;
    hash_t* p = (hash_t *)malloc( sizeof(hash_t) );
    assert( p != NULL );
    for( i=0; i<HASH_SIZE; i++ ) {
        p->table[i] = NULL;
    }
    p->next = NULL;
    p->scope_owner = NULL;
    return p;
}

void free_hash()
{ ; }

void print_hash( hash_t* p )
{
    int i;
    if( p == NULL) return;
    fprintf( stderr, ">>>BEGIN TABLE\n" );
    if(p->scope_owner != NULL) {
        fprintf( stderr, "Scope Owner: \n" );
        print_list(p->scope_owner);
    }
    for( i=0; i<HASH_SIZE; i++ ) {
        fprintf( stderr, "[%d]:", i );
        print_list( p->table[i] );
    }
    fprintf( stderr, "<<<END TABLE\n" );
}

/* search+insert over hash table */
list_t* search_hash( hash_t* scope, char* name )
{
    int index;
    list_t* p;

    assert( scope != NULL );
    index = hashpjw( name );
    p = scope->table[index];
    return search_list( p, name );
}

list_t* insert_hash( hash_t* scope, char* name )
{
    int index;
    list_t* p;

    assert( scope != NULL );
    index = hashpjw( name );
    p = scope->table[index];
    scope->table[index] = insert_list( p, name );
    return scope->table[index];

}

list_t* insert_hash_check( hash_t* scope, char* name )
{
    if(search_hash(scope, name) != NULL) {
        fprintf( stderr, "\nERROR: local object %s redeclared\n", name);
        exit(1);
    }
    insert_hash(scope, name);
}

list_t* get_objects( hash_t* scope )
{
    list_t* objects;
    list_t* p;
    int i;
    if( scope == NULL) return;
    fprintf(stderr, "\n");
    for( i=0; i<HASH_SIZE; i++ ) {
        if(scope->table[i] != NULL){
            p = copy_list( scope->table[i] );
            print_list(p);
            p = p->next;
        }
    }
    return objects;
}

list_t* get_locals( hash_t* top, char* name )
{
    hash_t* p = top;
    list_t* q;
    while( p != NULL ) {    /* search local scope */
        fprintf(stderr, "NAME: %s ?= %s\n", p->scope_owner->name, name );
        // if( strcmp(strdup(p->scope_owner->name), strdup(name)) == 0 )
        // {
        //     fprintf(stderr, "NAME: %s\n", name );
        //     return p;
        // }
        p = p->next;        /* go to the next scope (down the stack) */
    }
    return NULL;
}

hash_t* find_scope( hash_t* top_scope, char* name )
{
    hash_t* p = top_scope;
    while( p != NULL ) {
        int i;
        // print_hash(p);
        for( i=0; i<HASH_SIZE; i++ ) {
            if(p->table[i] != NULL)
                if( p->table[i]->name == name )
                    return p;
                    // fprintf(stderr, "%s\n", p->table[i]->name);
                // print_list(p->table[i]->name);
        }
        p = p->next;
    }
    return NULL;
}

/* stackable */
hash_t* push_scope( hash_t* top )//, list_t* owner )   /* Add scope_owner?? */
{
    hash_t* p = make_hash();
    // p->scope_owner = owner;
    assert( p != NULL );
    p->next = top;
    return p;
}

hash_t* pop_scope( hash_t* top )
{
    hash_t* p;

    if( top == NULL ) return NULL;
    p = top->next;
    free_hash( top );
    return p;
}

list_t* search_scopes( hash_t* top_scope, char* name )
{
    hash_t* p = top_scope;
    list_t* q;
    while( p != NULL ) {    /* search local scope */
        if( (q = search_hash( p, name )) != NULL )
            return q;       /* found name */
        p = p->next;        /* go to the next scope (down the stack) */
    }
    return NULL;
}

void print_scopes( hash_t* top_scope )
{
    hash_t* p = top_scope;
    while( p != NULL ) {
        print_hash( p );
        p = p->next;
    }
}
