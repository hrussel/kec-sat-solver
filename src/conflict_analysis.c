#include "conflict_analysis.h"

int* visited;

int* get_conflict_induced_cl( variable abs_literal, int* conflict_cl_length ) {
    
    clause* unit_conflict_clause =
                sat_st.impl_graph[abs_literal].conflictive_clause;
    
    memset(visited, 0, sat_st.num_vars*sizeof(int));
    
    //printf("Index conflict: %d %d\n", abs_literal, (int)sat_st.impl_graph[abs_literal].conflictive_clause);
    
    stack reachable;
    initialize_list(&reachable);
    
    list conflict_induced_cl;
    initialize_list(&conflict_induced_cl);
        
    if ( unit_conflict_clause == NULL ){
        *conflict_cl_length = 0;
        return NULL;
        //push(&conflict_induced_cl, &((decision_level_data*)top(&sat_st.backtracking_status))->assigned_literal);
    
    } else {
        
        // Push predecessors of the conflict literal
        //printf("Es implicada\n");
        
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
            
            int abs_pred = abs(pred);
            //printf("Visitando %d\n", abs_pred);
            
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
                        
                        if ( abs(pred_pred) != abs_pred ){
                            //printf("Variable %d es alcanzable\n", pred_pred);
                            
                            push(&reachable, &cl->literals[i]);
                        }
                    }
                }
            } else {
                pop(&reachable);
            }
            
            //printf("    Visitado %d\n", abs_pred);
        }
    }
    //printf("Grafo expandido\n");
    
    int i = 0;
    int* clause = (int*)malloc(conflict_induced_cl.size*sizeof(int));
    
    //while(conflict_induced_cl.size == 0 );
    
    printf("Conflictive clause (%d, %d):", abs_literal, conflict_induced_cl.size);
    
    
    while( !empty(&conflict_induced_cl) ){
        clause[i] = *((int*)top(&conflict_induced_cl));
        clause[i] = -clause[i];
        
        printf(" %d", clause[i]);
        
        pop(&conflict_induced_cl);
        i++;
    }
    
    printf("\n");
    
    
    *conflict_cl_length = i;
    
    return clause;
}

int analyze_conflict() {
    
    // Will hold the length of the conflict_induced_clause.
    int clause_length;
    int conflictive_variable;
    
    decision_level_data* dld = top(&sat_st.backtracking_status);
    
    if ( empty(&dld->propagated_var) ){
        conflictive_variable = abs(dld->assigned_literal);
    } else {
        conflictive_variable = abs(*(int*)top(&dld->propagated_var));
    }
    
    get_conflict_induced_cl( conflictive_variable, &clause_length );
    
    //int* learned_clause = get_conflict_induced_cl( &clause_length );
    //learn_clause( clause_length, learn_clause );

    return 1;
}


