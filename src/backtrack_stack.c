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
#include "backtrack_stack.h"


backtrack_struct* new_list_bt(){
    backtrack_struct *nl = (backtrack_struct*)malloc(sizeof(backtrack_struct));
    initialize_list_bt(nl);
    return nl;
}

void initialize_list_bt(backtrack_struct* l){
    l->first_free_pos = 0;
    l->stack = (decision_level_data*)
        malloc( (sat_st.num_vars+1)*sizeof(decision_level_data));
    // sin el 1.
}

void push_bt( backtrack_struct* st, decision_level_data* item ){
    
    if ( 0<= st->first_free_pos && st->first_free_pos< sat_st.num_vars ) {
        st->stack[st->first_free_pos] = *item;
        st->first_free_pos++;
    }
    else {
        report_io_error(3);
        exit(1);
    }
}

//@pre isNotEmpty(l)
decision_level_data* top_bt( backtrack_struct* l ){
    if ( !empty_bt(l) ){
        return (l->stack)+(l->first_free_pos)-1;
    }
    else {
        return NULL;
    }
}

//@pre isNotEmpty(l)
void pop_bt( backtrack_struct* l ){
    
    if ( !empty_bt(l) ){
        l->first_free_pos--;
    }
}

int empty_bt( backtrack_struct* l ){
    return l->first_free_pos == 0;
}

void multi_pop_bt( backtrack_struct* l, int n ){
    while( n-- && !empty_bt(l) ){
        pop_bt(l);
    }
}

void print_bt(backtrack_struct* l){
    int i;
    decision_level_data current_dec;
    printf("Size %d\n",(l->first_free_pos)-1);
    for ( i=0; i< l->first_free_pos; i++ ) {
        current_dec = (l->stack)[i];
        printf("Assigned literal: %d. Missing branch %d",
               current_dec.assigned_literal, current_dec.missing_branch );
    }
}

void free_bt( backtrack_struct* l ) {
    free(l->stack);
} 
