/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Singly-linked list and all its common operations.
 *
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/

#ifndef _KECOSATS_LIST_

#define _KECOSATS_LIST_

#include <stdlib.h>
#include <stdio.h>

#define TRUE 1
#define FALSE 0

typedef struct node_{
    void* item;
    struct node_ *next, *prev;
} node;
    
typedef struct list_{
    struct node_* first;
    struct node_* last;
    int size;
} list;

list* new_list();

void initialize_list(list* l);

void push(list* l, void* item);

void* top(list* l);

void pop(list* l);

void multi_pop(list* l, int n);

void queue(list* l, void* item);

void dequeue(list* l, void* item);

int empty(list* l);

#endif
