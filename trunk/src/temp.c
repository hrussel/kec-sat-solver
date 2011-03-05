//typedef struct {
//    int decision_level;
//    clause* conflictive_clause; // o el indice de la clausula?
//} decision_node;
#include "kecosats_structures.h"
#include <string.h>
#include "list.h"

//decision_node impl_graph[sat_st.num_vars];


int get_bt_target_level( variable literal ) {
    
    variable abs_literal = abs(literal);
    
    clause* conflict_clause = sat_st.impl_graph[abs_literal].conflictive_clause;
    
    sat_st.impl_graph[abs_literal].decision_level = -1;
    
    {
        int i, current_max=-1;
        decision_node* current_dec_node;
        
        int cl_size = conflict_clause->size;
        
        for( i=0; i<cl_size; i++ ) {
            current_dec_node = sat_st.impl_graph
                                + abs(conflict_clause->literals[i]);
            
            if( current_max < current_dec_node->decision_level ) {
                current_max = current_dec_node->decision_level;
            }
        }
        
        return current_max;
    }
}

int* visited;

int* get_conflict_induced_cl( variable literal, int* conflict_cl_length ) {
    
    variable abs_literal = abs(literal);
    clause* unit_conflict_clause =
                sat_st.impl_graph[abs_literal].conflictive_clause;
    
    memset(visited, 0, sat_st.num_vars*sizeof(int));
    
    // Mark each reachable literal as visited
    
    stack reachable;
    initialize_list(&reachable);
    
    list conflict_induced_cl;
    initialize_list(&conflict_induced_cl);
    
    // Push predecessors of the conflict literal
    
    {
        
        int cl_size = unit_conflict_clause->size;
        
        int i;
        for( i=0; i<cl_size; i++ ) {
            int pred = unit_conflict_clause->literals[i];
            
            if ( abs(pred) != abs_literal ){
                push(&reachable, &unit_conflict_clause->literals[i]);
            }
        }
    }
    
    while(!empty(&reachable)){
        int pred = *((int*)top(&reachable));
        
        int abs_pred = abs(pred);
        
        if ( !visited[abs_pred] ){
            visited[abs_pred] = TRUE;
            
            if ( sat_st.impl_graph[abs_pred].conflictive_clause ){
                push(&conflict_induced_cl, top(&reachable));
            }
        }
        
        pop(&reachable);
    }
    
    int i = 0;
    int* clause = (int*)malloc(reachable.size*sizeof(int));
    
    while( !empty(&reachable) ){
        clause[i] = *((int*)top(&reachable));
        clause[i] = -clause[i];
        
        pop(&reachable);
        i++;
    }
    
    *conflict_cl_length = i;
    return clause;
}

int main(){ return 0; }