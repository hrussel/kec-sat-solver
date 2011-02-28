#include"kecosats_structures.h"


inline void add_to_watched_list(variable v, clause* cl){
    
    if ( v > 0 ){
        push(&sat_st.pos_watched_list[abs(v)], cl);
    } else {
        push(&sat_st.neg_watched_list[abs(v)], cl);
    }
}


int is_satisfied( variable v ){
    
    int abs_v = abs(v);
    
    if ( sat_st.model[ abs_v ] == UNKNOWN ){
        return FALSE;
    }
    
    if ( (sat_st.model[ abs_v ] == FALSE && v < 0)
            || (sat_st.model[ abs_v ] == TRUE && v > 0)
        )
    {
        return TRUE;
    }
    
    return FALSE;
}

int is_head_watcher( clause* cl, variable literal ){
    return abs(*(cl->head_watcher)) == abs(literal);
}

int is_tail_watcher( clause* cl, variable literal ){
    return abs(*(cl->tail_watcher)) == abs(literal);
}

void swap_watchers( clause* cl ){
    variable* tmp = cl->head_watcher;
    cl->head_watcher = cl->tail_watcher;
    cl->tail_watcher = tmp;
}


int update_watcher( clause* clause ) {
    
    variable current_literal;
    
    // This variable will indicate if in the clause @head_clause there's an
    // unassigned literal.
    int exists_free_literal = FALSE;  
    
    {
        int i;
        // Iterate over the clause head_clause searching for an unassigned
        // literal not pointed to by the tail_watcher. If it finds one such
        // unassigned literal, make the head_watcher point to it.
        for ( i=0; i < clause->size; i++ ){
            current_literal = abs( clause->literals[i] );
            
            if ( (sat_st.model[current_literal] == UNKNOWN
                    || is_satisfied(clause->literals[i]))
                && clause->tail_watcher != clause->literals + i )
            {
                // @Assert: There's an unassigned literal not pointed to by the
                // tail_watcher.
                clause->head_watcher = (clause->literals) + i;
                exists_free_literal = TRUE;
                break;
            }
        }
    }
    
    if ( !exists_free_literal ) {
        
        // @Assert: Either all literals are assigned or the only unassigned
        // literal is pointed to by the tail_watcher.
        
        // assign( *(head_clause->tail_watcher), TRUE );
        
        if ( current_literal_value(clause->tail_watcher) == UNKNOWN ) {
          // @Assert: This clause is a unit clause.
            return UNIT_CLAUSE;
        } else if ( is_satisfied(*(clause->tail_watcher)) ){
            return DONT_CARE;
        }
        
        // @Assert: This clause is not satisfiable. The literal pointed to by
        // the tail_watcher is already assigned and evaluates to False. 
        // We require here that head_clause->satisfied != TRUE.
        return CONFLICT;
    }
    // @ Assert: We found an unassigned literal distinct not pointed to by the
    // tail_watcher. 
    return DONT_CARE;
}

int current_literal_value( variable* literal ) {
  return sat_st.model[abs(*literal)];
}

