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

        if ( current_clause.satisfied != TRUE ) {
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
        }
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

int  eliminate_pure_literals( variable* pure_literals, 
                              int pure_literals_size ) {
    int i;
    int status = DONT_CARE;

    for( i=0; i< pure_literals_size; i++ ) {
        status = deduce( pure_literals[i] );
    }
    free( pure_literals );
    return status;
}
