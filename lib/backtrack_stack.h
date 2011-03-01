#include "kecosats_structures.h"

//@ inv 0<=first_free_pos <= sat_st.num_var;
typedef struct backtrack_stack{
    int first_free_pos;
    decision_level_data* stack;
} backtrack_struct;


backtrack_struct* new_list_bt();

void initialize_list_bt(backtrack_struct* l);

void push_bt( backtrack_struct* st, decision_level_data* item );

decision_level_data* top_bt( backtrack_struct* l );

void pop_bt( backtrack_struct* l );

int empty_bt( backtrack_struct* l );

void multi_pop_bt( backtrack_struct* l, int n );

void print_bt(backtrack_struct* l);

void free_bt( backtrack_struct* l );
