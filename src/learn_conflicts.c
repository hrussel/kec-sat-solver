void learn_new_clause( int clause_length, int lit[] ){
    if ( sat_st.left_space == 0 ) {
        // Get more space
        sat_st.formula = (clause*) realloc(sat_st.formula,
                                           2*num_clauses*sizeof(clause));

        sat_st.pos_watched_list = (list*) realloc(sat_st.pos_watched_list,
                                           2*num_clauses*sizeof(list));

        sat_st.neg_watched_list = (list*) realloc(sat_st.neg_watched_list,
                                           2*num_clauses*sizeof(list));
    }

    //    sat_st.num_original_clauses;
    sat_st.num_clauses++;
    sat_st.left_space--;
    // Set watchers lists.
    set_clause( &sat_st.formula[sat_st.num_clauses], clause_length, lit );    
}

void delete_clause( int clause_index ) {
    int last_clause_index = sat_st.num_clauses-1;
    swap_clauses( sat_st.formula, clause_index, last_clause_index );
    free( sat_st.formula[last_clause_index]->literals );
    sat_st.num_clauses--;
}
