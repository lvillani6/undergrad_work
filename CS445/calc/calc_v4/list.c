#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "list.h"


list_t *make_list( char *name )
{
    list_t *p = (list_t *)malloc( sizeof(list_t) );
    assert( p != NULL );
    p->name = strdup( name );
    p->value = 0;
    p->next = NULL;
    return p;
}

void free_list( list_t *top )
{
    top = NULL;   /* buggy and bad */
}

void print_list( list_t *top )
{
    list_t *p = top;
    while( p != NULL ) {
        fprintf( stderr, "[%s]", p->name );
        p = p->next;
    }
    fprintf( stderr, "\n" );
}

list_t *search_list( list_t *top, char *name )
{
    list_t *p = top;
    if( top == NULL ) return NULL;
    while( p != NULL ) {
        if( !strcmp(p->name, name ) )
            return p;
        p = p->next;
    }
    return NULL;
}

list_t *insert_list( list_t *top, char *name )
{
    list_t *p = make_list( name );
    assert( p != NULL );
    if( top == NULL )   /* linked list is empty */
        return p;
    else {              /* insert new node at the top */
        p->next = top;
        return p;
    }
}

void delete_list( list_t *top, char *name )
{
    ;
}
