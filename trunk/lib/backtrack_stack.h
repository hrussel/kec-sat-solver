#include "kecosats_structures.h"

//@ inv 0<=first_free_pos <= sat_st.num_var;

backtrack_stack *new_list_bt();

void initialize_list_bt(backtrack_stack *l);

void push_bt( backtrack_stack *st, decision_level_data *item );

decision_level_data* top_bt( backtrack_stack *l );

void pop_bt( backtrack_stack *l );

inline int empty_bt( backtrack_stack *l );

void multi_pop_bt( backtrack_stack *l, int n );

void empty_the_stack( backtrack_stack *l );

void print_bt(backtrack_stack* l);

void free_bt( backtrack_stack* l );
