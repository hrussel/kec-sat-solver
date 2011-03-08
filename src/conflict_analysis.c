#include "conflict_analysis.h"

int* visited;

/* OBS: Se quito que considerase una clausula de las conocidas dado que podemos
        hacer el backtracking no cronologico aun sin aprender dicha clausula
 */
int get_bt_target_level( int clause_length, int* conflict_clause ) {
    int i, current_max=-1;
    decision_node* current_dec_node;
    
    for( i=0; i<clause_length; i++ ) {
        current_dec_node = sat_st.impl_graph + abs(conflict_clause[i]);
        current_max = max(current_max, current_dec_node->decision_level);
        
        
    }
    
    return current_max;
}

int* get_conflict_induced_cl( variable abs_literal, int* conflict_cl_length ) {
    
    clause* unit_conflict_clause =
                sat_st.impl_graph[abs_literal].conflictive_clause;
    
    memset(visited, 0, (sat_st.num_vars+1)*sizeof(int));
    
    //printf("Index conflict: %d %d\n", abs_literal, (int)sat_st.impl_graph[abs_literal].conflictive_clause);
    
    stack reachable;
    initialize_list(&reachable);
    
    list conflict_induced_cl;
    initialize_list(&conflict_induced_cl);
    
    if ( unit_conflict_clause == NULL ){
        *conflict_cl_length = 0;
        return NULL;
    }
    
    //printf("la clausula de %d es %d\n", abs_literal, unit_conflict_clause - sat_st.formula);
    // Set directed predecessors reachables
    {
        
        int cl_size = unit_conflict_clause->size;
        
        int i;
        for( i=0; i<cl_size; i++ ) {
            int pred = unit_conflict_clause->literals[i];
            
            if ( abs(pred) != abs_literal ){
                //printf("Variable %d es alcanzable\n", pred);
                
                push(&reachable, &unit_conflict_clause->literals[i]);
            }
        }
    }
    
    while(!empty(&reachable)){
        int pred = *((int*)top(&reachable));
        //printf("            PRED: %d\n", pred);
        
        int abs_pred = abs(pred);
        
        if ( !visited[abs_pred] ){
            visited[abs_pred] = TRUE;
            
            if ( sat_st.impl_graph[abs_pred].conflictive_clause == NULL){
                
                push(&conflict_induced_cl, top(&reachable));
                pop(&reachable);
                
            } else {
                pop(&reachable);
                
                clause *cl = sat_st.impl_graph[abs_pred].conflictive_clause;
                
                int cl_size = cl->size;
                
                int i;
                for( i=0; i<cl_size; i++ ) {
                    int pred_pred = cl->literals[i];
                    
                    if ( abs(pred_pred) != abs_pred 
                            && !visited[abs(pred_pred)])
                    {
                        push(&reachable, &cl->literals[i]);
                    }
                }
            }
        } else {
            pop(&reachable);
        }
    }
    
    int i = 0;
    int* clause = (int*)malloc(conflict_induced_cl.size*sizeof(int));
    
    while( !empty(&conflict_induced_cl) ){
        clause[i] = *((int*)top(&conflict_induced_cl));
        
        if ( sat_st.model[clause[i]] > 0 ){
            clause[i] = -abs(clause[i]);
        } else {
            clause[i] = abs(clause[i]);
        }
        
        printf(" %d", clause[i]);
        
        pop(&conflict_induced_cl);
        i++;
    }
    printf(" 0\n");
    
    *conflict_cl_length = i;
    
    return clause;
}

int analyze_conflict(int** conflictive_clause, int* clause_length) {
    
    // Will hold the length of the conflict_induced_clause.
    
    int conflictive_variable;
    
    decision_level_data* dld = top(&sat_st.backtracking_status);
    
    if ( empty(&dld->propagated_var) ){
        conflictive_variable = abs(dld->assigned_literal);
    } else {
        conflictive_variable = abs(*(int*)top(&dld->propagated_var));
    }
    
    *conflictive_clause = 
                get_conflict_induced_cl( conflictive_variable, clause_length );
    
    if ( *clause_length > 0 ){
        int target_level = get_bt_target_level(*clause_length, *conflictive_clause);
        
        /*
        if ( sat_st.backtracking_status.size - target_level > 20 ){
            printf("Target [%d] : %d -> %d\n",
                *clause_length, sat_st.backtracking_status.size, target_level);
            int aux;
            scanf("%d", &aux);
        }
        */
        return target_level;
    }
    
    return sat_st.backtracking_status.size;
    //return learn_clause( clause_length, learned_clause );
}


