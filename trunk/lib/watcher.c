#include "sat.h"
#include "list.h"
#include <stdlib.h>


// OJO: Esto no está completo, falta escribir bien cómo va a retornar los
// parámetros y qué hace con ellos la función que llama a update_watcher.
int update_watcher( clause* head_clause ) {
  variable current_literal;
  head_clause->head_watcher = head_clause->tail_watcher;

  {
    int i;
    for ( i=0; i< head_clause->size; i++ ){
      current_literal = abs( head_clause->literals[i] );
    
      if ( sat_st.model[current_literal] != UNKNOWN ) {
        head_clause->head_watcher = (head_clause->literals) + i;
        break;
      }
    }
  }

  // ESTO ES LO QUE HAY QUE MEJORAR.
  if ( head_clause->head_watcher == head_clause->tail_watcher ) {
    // The only literal not set to 0 in the clause is the other
    // watcher.
    // This clause is a unit clause.
    // assign( *(head_clause->tail_watcher), TRUE );
    
    if ( current_literal_value(head_clause->tail_watcher) == FALSE ) {
      // This clause is not satisfiable.
      return CONFLICT;
    }
    
    return UNIT_CLAUSE;
  }
  
  return DONT_CARE;
}
