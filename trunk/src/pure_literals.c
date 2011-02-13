#include "pure_literals.h"
#include <stdio.h>

typedef struct {
    int polarity;
    int is_pure_literal;
} pure_literal_status;



//@pre (%forall i:0<=i<sat.st.num_vars: pure_literals[i].pure_literal =UNKNOWN);


variable* find_pure_literals( int* pure_literals_size ) {
    int i=0,j=0;
    variable literal;
    int abs_literal;
    clause current_clause;
    pure_literal_status pl_status[sat_st.num_vars+1];    
    variable* pure_literals =
      (variable*) malloc( (sat_st.num_vars+1)*sizeof(variable));
    int curr_literal_polarity;

    *pure_literals_size = 0;
    memset( pl_status, -1, (sat_st.num_vars+1)*sizeof(pure_literal_status));

    for ( i=0; i< sat_st.num_clauses; i++ ) {
        current_clause = sat_st.formula[i];
        //@pre current_clause is satisfied.
        //if ( current_clause.satisfied != TRUE ) {
            for ( j=0; j< current_clause.size; j++ ) {
                literal = current_clause.literals[j];
                abs_literal = abs( literal );

                // If current literal is known already to be an impure literal,
                // proceed.
                if ( pl_status[abs_literal].is_pure_literal == FALSE ) {
                    continue;
                }
                else if (pl_status[abs_literal].is_pure_literal == UNKNOWN){
                    curr_literal_polarity = abs_literal/literal;
                    pl_status[abs_literal].polarity =
                      curr_literal_polarity;
                    pl_status[abs_literal].is_pure_literal = TRUE;
                }
                else { 
                    // @Assert: pl_status[abs_literal].pure_literal == TRUE;
                    curr_literal_polarity = abs_literal/literal;
                    if ( pl_status[abs_literal].polarity !=
                         curr_literal_polarity ){
                      printf("%d no es puro \n", abs_literal);
                      pl_status[abs_literal].is_pure_literal = FALSE;
                    }
                }
            }
            //}
    }

    for ( i=1; i<= sat_st.num_vars; i++ ) {
        if ( pl_status[i].is_pure_literal == TRUE ) {
            pure_literals[*pure_literals_size] = i*pl_status[i].polarity;
            printf("%d es puro\n", i*pl_status[i].polarity);
            (*pure_literals_size)++;
        }
    }

    return pure_literals;
}

//int  eliminate_pure_literals( variable* pure_literals, 
//                              int pure_literals_size ) {
//    int i;
//    int status = DONT_CARE;
//
//    for( i=0; i< pure_literals_size; i++ ) {
//      // This should be a rewriting of the formula instead of
//      // a deduce.
//        status = deduce( pure_literals[i] );
//    }
//    free( pure_literals );
//    return status;
//}
/**
 *
 *
 * Note: One minor inconvenience with this function is that it
 *       alters the order of the literals in a clause and the order
 *       of appearance of the clauses in a formula.
 *
 */
int  eliminate_pure_literals( variable* pure_literals, 
                              int num_pure_literals ) {
    int i,j;
    int status = DONT_CARE;
    int abs_literal;
    variable literal;
    int curr_literal_polarity;
    clause* current_clause;

    // For each pure literal, update the model.
    for( i=0; i<num_pure_literals; i++ ) {
        literal = pure_literals[i];
        abs_literal = abs( literal );
        curr_literal_polarity = abs_literal/literal;

        sat_st.model[abs_literal] = curr_literal_polarity;
    }

    for ( i=0; i< sat_st.num_clauses; i++ ) {
        current_clause = sat_st.formula + i;

        for ( j=0; j< current_clause->size; j++ ) {
            literal = current_clause->literals[j];
            abs_literal = abs( literal );

            if ( is_pure_literal(literal, pure_literals, num_pure_literals) ) {
                // If the clause is a unitary clause and it's single literal
                // is a pure one. Eliminate the clause.
                if ( current_clause->size == 1 ) {

                    if ( sat_st.num_clauses <= 1 ) {
                        return SATISFIED;
                    }
                    // If there's another clause of the formula still
                    // unexamined.
                    if ( j-i > 1 ) {
                        swap_clauses( sat_st.formula, j, sat_st.num_clauses-1 );
                    }
                    
                    sat_st.num_clauses--;
                    free( current_clause->literals );
                }        
                else if ( current_clause->size > 1 ) {
                    // If there's another literal of the clause still
                    // unexamined.
                    if ( current_clause->size -j> 1 ) {
                        swap_literals( current_clause, j, current_clause->size);
                    }
                    (current_clause->size)--;
                }
            }
        }
    }
}

void swap_literals( clause* cl, int old_index, int new_index ) {
    variable temp_literal;
    temp_literal = (cl->literals)[old_index];
    (cl->literals)[old_index] = (cl->literals)[new_index];
    (cl->literals)[new_index] = temp_literal;
}

void swap_clauses( clause* formula, int old_index, int new_index ) {

}

int is_pure_literal( variable literal, variable* pure_literals,
                     int num_pure_literals ) {
    int i;
    for( i=0; i< num_pure_literals; i++ ) {

        if ( literal == pure_literals[i] ) {
            return TRUE;
        }
    }
    return FALSE;
}
