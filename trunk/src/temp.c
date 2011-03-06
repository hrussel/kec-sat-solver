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

int main(){ return 0; }