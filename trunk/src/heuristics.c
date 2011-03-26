/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Source code for the clause learning procedures of kecosats solver.
 *
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/


#include "heuristics.h"

int decide_greedy(){
    
    variable free_variable=1;
    
    while(  free_variable <= sat_st.num_vars &&
        sat_st.model[free_variable] != UNKNOWN )
    {
        free_variable++;
    }
    
    return free_variable;
}

int decide_berkmin(){
    
    variable free_variable=0;
    double max_activity = -1.0;
    int i;
    
    if ( sat_st.clause_available_space ){
        
        clause* current_top_clause = &sat_st.formula[ sat_st.num_clauses - 1];
        for ( i=0; i< current_top_clause->size; i++){
            if ( sat_st.model[abs(current_top_clause->literals[i])]
                    == UNKNOWN ){
                
                variable v = abs( current_top_clause->literals[i] );
                
                if ( sat_st.ac[v] > max_activity ){
                    free_variable = v;
                }
            }
        }
    }
    
    if ( !free_variable ){
        
        for ( i=1; i <= sat_st.num_vars; i++){
            if ( sat_st.ac[i] > max_activity 
                  && sat_st.model[i] == UNKNOWN
                )
            {
                free_variable = i;
            }
        }
        
        
        if ( free_variable ){
            return free_variable;
        }
        
    }
    
    if ( !free_variable ){
        return sat_st.num_vars+1;
    } else {
        
        if ( sat_st.lit_ac[free_variable] < 0 ){
            free_variable = -free_variable;
        }
        
        return free_variable;
    }
}

void initialize_kecosats_heuristic(){
    int cl, i;
    
    for ( cl=0; cl<sat_st.num_clauses; cl++){
        for (i=1; i<=sat_st.formula->size; i++){
            
            if ( sat_st.formula->size == 2 ){
                sat_st.ac[ abs(sat_st.formula->literals[i]) ]++;
            }
            
            if ( sat_st.formula->literals[i] >= 0 ){
                sat_st.lit_ac[ abs(sat_st.formula->literals[i]) ]++;
            } else {
                sat_st.lit_ac[ abs(sat_st.formula->literals[i]) ]--;
            }
        }
    }
    
}

int decide_kecosats(){
    
    variable free_variable=0;
    double max_activity = -1.0;
    int i;
    
    if ( sat_st.clause_available_space ){
        
        clause* current_top_clause = &sat_st.formula[ sat_st.num_clauses - 1];
        for ( i=0; i< current_top_clause->size; i++){
            if ( sat_st.model[abs(current_top_clause->literals[i])]
                    == UNKNOWN )
            {
                
                variable v = abs( current_top_clause->literals[i] );
                
                if ( sat_st.ac[v] > max_activity ){
                    free_variable = v;
                }
            }
        }
    }
    
    if ( !free_variable ){
        
        for ( i=1; i <= sat_st.num_vars; i++){
            if ( sat_st.ac[i] > max_activity 
                  && sat_st.model[i] == UNKNOWN
                )
            {
                free_variable = i;
            }
        }
        
    }
    
    if ( !free_variable ){
        return sat_st.num_vars+1;
    } else {
        
        if ( sat_st.pos_watched_list[free_variable].size <
                sat_st.neg_watched_list[free_variable].size
            ){
            free_variable = -free_variable;
        }
        
        return free_variable;
    }
}
