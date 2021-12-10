#ifndef LIST_H
#define LIST_H

typedef struct list_s {
    char *name;     /* attribute for ID */
    int value;      /* value for ID */
    struct list_s *next;    /* singly linked list */
}
list_t;

list_t *make_list( char *name );
void free_list( list_t *top );
void print_list( list_t *top );       /* visual debug */

list_t *search_list( list_t *top, char *name );
list_t *insert_list( list_t *top, char *name );
void delete_list( list_t *top, char *name );

#endif
