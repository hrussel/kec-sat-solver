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

backtrack_stack* new_list_bt(){
    backtrack_stack *nl = (backtrack_stack*)malloc(sizeof(backtrack_stack));
    initialize_list_bt(nl);
    return nl;
}

void initialize_list_bt(backtrack_stack* l){
    l->first_free_pos = 0;
    l->stack = (decision_level_data*)
        malloc( (sat_st.num_vars+1)*sizeof(decision_level_data));
    // sin el 1.
}

void push_bt( backtrack_stack* st, decision_level_data* item ){
    
    if ( st->first_free_pos< sat_st.num_vars ) {
        st->stack[st->first_free_pos] = *item;
        st->first_free_pos++;
    }
    else {
        //report_io_error(3);
        exit(1);
    }
}

//@pre isNotEmpty(l)
decision_level_data* top_bt( backtrack_stack* l ){
    if ( empty_bt(l) ){
        return NULL;
    }
    else {
        return (l->stack)+(l->first_free_pos)-1;
    }
}

//@pre isNotEmpty(l)
void pop_bt( backtrack_stack* l ){
    
    if ( !empty_bt(l) ){
        l->first_free_pos--;
    }
}

inline int empty_bt( backtrack_stack* l ){
    return l->first_free_pos == 0;
}

void multi_pop_bt( backtrack_stack* l, int n ){
    while( n-- && !empty_bt(l) ){
        pop_bt(l);
    }
}

void empty_the_stack( backtrack_stack* l ) {
    l->first_free_pos = 0;
}

void print_bt(backtrack_stack* l){
    int i;
    decision_level_data current_dec;
    printf("Size %d\n",(l->first_free_pos)-1);
    for ( i=0; i< l->first_free_pos; i++ ) {
        current_dec = (l->stack)[i];
        printf("Assigned literal: %d. Missing branch %d",
               current_dec.assigned_literal, current_dec.missing_branch );
    }
}

void free_bt( backtrack_stack* l ) {
    free(l->stack);
    free(l);
} 

//int main() {
//    sat_st.num_vars = 8;
//    printf("llegue\n");
//    backtrack_stack* bt = new_list_bt();
//    initialize_list_bt( bt );
//    decision_level_data dlevel1;
//    dlevel1.assigned_literal = -8;
//    dlevel1.missing_branch = TRUE;
//
//    decision_level_data dlevel2;
//    dlevel2.assigned_literal = -7;
//    dlevel2.missing_branch = FALSE;
//
//    decision_level_data dlevel3;
//    dlevel3.assigned_literal = 6;
//    dlevel3.missing_branch = TRUE;
//
//    decision_level_data dlevel4;
//    dlevel4.assigned_literal = -5;
//    dlevel4.missing_branch = FALSE;
//
//    push_bt( bt, &dlevel1 );
//    push_bt( bt, &dlevel2 );
//    push_bt( bt, &dlevel3 );
//    push_bt( bt, &dlevel4 );
//    printf("llegue2\n");
//    decision_level_data* eltope = top_bt( bt );
//
//    printf("el tope:\n Asigna: %d y missing: %d\n", eltope->assigned_literal,
//           eltope->missing_branch);
//
//    while ( !empty_bt(bt) ){
//        eltope = top_bt( bt );
//        printf("el tope2:\n Asigna: %d y missing: %d\n", eltope->assigned_literal,
//           eltope->missing_branch);
//        pop_bt( bt );
//    }
//
//    return 0;
//}
