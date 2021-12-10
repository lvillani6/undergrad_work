#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "list.h"

#define BUFF_SIZE 32

int main()
{
    int choice;
    char buffer[BUFF_SIZE];
    list_t *top = NULL;     /* start of linked list */
    list_t *p;

    while(1) {
        fprintf( stderr, "0.search 1.insert 2.print: " );
        scanf("%d", &choice );
        switch ( choice ) {
            case 0:
                scanf( "%s", buffer );
                p = search_list( top, buffer );
                if( p != NULL )
                    fprintf( stderr, "Found[%s]\n", p->name );
                else
                    fprintf( stderr, "NotFound[%s]\n", buffer );
                break;
            case 1:
                scanf( "%s", buffer );
                top = insert_list( top, buffer );
                break;
            case 2:
                print_list( top );
                break;
        }
    }
}
