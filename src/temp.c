typedef struct {
    int decision_level;
    clause* conflictive_clause; // o el indice de la clausula?
} decision_node;

decision_node impl_graph[sat_st.num_vars];


int get_backtracking_level( var literal ) {
    variable abs_literal = abs(dec_lev_dat->assigned_literal);

    clause* conflict_clause = impl_graph[abs_literal].conflictive_clause;

    // impl_graph[abs_literal].decision_level = 0
    // esto nos ahorra la 2da rama del if de abajo?
    {
        int i, current_max=-1;
        decision_node* current_dec_node;
        for( i=0; i<conflict_clause->size; i++ ) {
            current_dec_node = impl_graph + abs(conflict_clause->literals[i]);

            if( current_max < current_dec_node->decision_level 
                &&
                abs(conflict_clause->literals[i]) != abs_literal ) {
                current_max = current_dec_node->decision_level;
            }
        }

    }
    return current_max;
}

int* get_conflict_induced_cl( var literal, int* conflict_cl_length ) {


}
