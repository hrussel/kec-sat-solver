//#include "sat.h"
variable* find_pure_literals( int* pure_literals_size );

void swap_literals( clause* cl, int old_index, int new_index );

void swap_clauses( clause* formula, int old_index, int new_index );

int is_pure_literal( variable literal, variable* pure_literals,
                     int num_pure_literals );
