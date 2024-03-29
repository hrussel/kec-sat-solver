/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Definition of the functions of the linked list. 
 *
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/
#include "list.h"

list* new_list(){
    list *nl = (list*)malloc(sizeof(list));
    initialize_list(nl);
    return nl;
}

void initialize_list(list* l){
    l->first = l->last = NULL;
    l->size = 0;
}

void push(list* l, void* item){
    
    node* new_node;
    
    new_node = (node*)malloc(sizeof(node));
    
    if (new_node == NULL){
        printf("Error: malloc\n");
        exit(1);
    }
    
    new_node->item = item;
    new_node->next = l->first;
    new_node->prev = NULL;
    
    if (!empty(l)){
        l->first->prev = new_node;
    } else {
        l->last = new_node;
    }
    
    l->first = new_node;
    l->size++;
}

inline void* top(list* l){
    if (empty(l)){
        return 0;
    } else {
        return l->first->item;;
    }
}

void pop(list* l){
    
    if (!empty(l)){
        
        node* next = l->first->next;
        
        free(l->first);
        
        l->first = next;
        
        if (l->first != NULL){
            l->first->prev = NULL;
        }
        
        l->size--;
        
        if (empty(l)){
            l->last = NULL;
        }
    }
}

inline int empty(list* l){
    return l->size == 0;
}

void multi_pop(list* l, int n){
    while(n-- && !empty(l)){
        pop(l);
    }
}

void queue(list* l, void* item){
    node* new_node;
    
    new_node = (node*)malloc(sizeof(node));
    new_node->item = item;
    new_node->prev = l->last;
    new_node->next = NULL;
    
    if (l->last){
        l->last->next = new_node;
    }
    
    l->last = new_node;
    
    if (empty(l)){
        l->first = new_node;
    }
    
    l->size++;
}

void dequeue(list* l, void* item){
    if (l->last){
        
        node* next = l->first->next;
        
        free(l->first);
        
        l->first = next;
        
        if (l->first){
            l->first->prev = 0;
        }
        
        l->size--;
        
        if (l->size == 0){
            l->last = 0;
        }
    }
}

void print(list* l){
    node* next = l->first;
    printf("Size %d\n",l->size);
    while(next){
        printf("%d\n", *((int*)next->item));
        next = next->next;
    }
}
