#include "sat.h"

/**
 * The purpose of this function is to initialize a clause from an array
 * of integers which represent the literals occurring in the aforementioned
 * clause.
 *
 * Note: It may occur that lit is an array with a length larger than the
 *       number of literals.
 *
 * @param cl A pointer to an unitialized clause.
 * @param clause_length The number of literals that occur in the clause
 *        pointed to by cl.
 * @param lit An integer array with numbers which represent the literals
 *        that occur in the single clause pointed to by cl.
 *
 */
void set_clause(clause* cl, int clause_length, int lit[]){
    
    cl->size = clause_length;
    
    cl->literals = (variable*) malloc (clause_length*sizeof(variable));
    memcpy( cl->literals, lit, clause_length*sizeof(int) );
    
    cl->head_watcher = cl->literals;
    
    /* OJO: en este punto habria que chequear que no aparezca una variable
            varias veces, si no podriamos eliminar ocurrencias o hacer true
            la clausula
     */
    
    if ( clause_length > 1 ){
        cl->tail_watcher = cl->literals + 1;
    } else {
        cl->tail_watcher = NULL;
    }
    
    if ( clause_length > 0 ){
        cl->satisfied = FALSE;
    } else {
        cl->satisfied = TRUE;
    }
}

/**
 * This function allocates memory for the elements that constitute
 * the global structure 'sat_st'.
 *
    */
void allocate_sat_status(){
    // Allocate space for the boolean formula.
    sat_st.formula = (clause*) malloc ( sat_st.num_clauses*sizeof(clause) );
   
    // Allocate space for the lists that keep track of the positive and
    // negative occurrences of each variable in the formula.
    sat_st.pos_occurrence_list = (list*) malloc( sat_st.num_vars*sizeof(list) );
    sat_st.neg_occurrence_list = (list*) malloc( sat_st.num_vars*sizeof(list) );
    
    memset( sat_st.pos_occurrence_list, 0, sizeof sat_st.pos_occurrence_list);
    memset( sat_st.neg_occurrence_list, 0, sizeof sat_st.neg_occurrence_list);

    // Allocate space for the model: the current assignment of truth values
    // to literals that is being studied.
    sat_st.model = (char*)malloc( sat_st.num_vars*sizeof(char) );
    memset(sat_st.model, UNKNOWN, sizeof(sat_st.model));
    
    if (   sat_st.formula 
        && sat_st.pos_occurrence_list
        && sat_st.neg_occurrence_list
        && sat_st.model
            == FALSE
        )
    {
        printf("Error: Couldn't allocate memory\n");
        exit(1);
    }
}

/**
 * This function parses a file that contains -- in DIMACS format--
 * the boolean formula that we intend to solve. It then allocates
 * space for the global structure sat_st and initializes its elements
 * with the information just collected from the file.
 *
 * @param filename The name of the file that contains the boolean
 *        formula (in DIMACS format) we intend to read.
 * @see  DIMACS reference...
 */
void set_initial_sat_status(char filename[]){
    
    char *buffer;
    int clauses, nbytes, current_literal, clause_length;
    int *clause_buffer;
    FILE * file;
    
    file = fopen (filename,"r");
    if ( file == NULL ){
        printf("Error: Couldn't open file\n");
        exit(1);
    }
    
    buffer = (char*) malloc( (BUFFERSIZE + 1)*sizeof(char) );    
    if ( buffer == NULL ){
        printf("Error: Couldn't allocate memory\n");
        exit(1);
    }
    
    nbytes = BUFFERSIZE;
    
    // Skip comments.
    while ( getline (&buffer, &nbytes, file) && buffer[0] == 'c');
    
    // We read the number of variables and clauses.
    sscanf( buffer, "p cnf %d%d", &sat_st.num_vars, &sat_st.num_clauses);
    
    clause_buffer = (int*) malloc( 2*sat_st.num_vars*sizeof(int) );
    
    // We allocate memory for each of sat_st's internal structures. 
    allocate_sat_status();
    
    clauses = 0;
    while ( clauses < sat_st.num_clauses ){
        
        char* l_aux;
        
        getline (&buffer, &nbytes, file);        
        if (*buffer == 'c')
            continue;
        
        l_aux = buffer;
        
        clause_length = 0;
        
        while ( sscanf(l_aux, "%d", &current_literal) && current_literal != 0 )
        {
            
            clause_buffer[clause_length] = current_literal;
            
            if ( current_literal > 0 ){
                // If the current_literal is >0, then in the current clause 
                // there is a positive occurrence of var current_literal.
                // We add the current clause to the current_literal's 
                // pos_occurrence_list.
                push( &sat_st.pos_occurrence_list[current_literal],
                      &sat_st.formula[clauses] );
            } else {
                // If the current_literal is <0, then in the current clause 
                // there is a positive occurrence of var abs(current_literal)
                // We add the current clause to the current_literal's 
                // neg_occurrence_list.
                push( &sat_st.neg_occurrence_list[-current_literal],
                      &sat_st.formula[clauses] );
            }
            
            // We search for the next literal in the buffer.
            // A literal corresponds to a number in the buffer.
            while ( isdigit(*l_aux) || *l_aux == '-')
                l_aux++;
            while ( !( isdigit(*l_aux) || *l_aux == '-') )
                l_aux++;
            
            clause_length++;
        }
        // We copy the array of literals just read (clause_buffer) in the
        // current line to one of sat_st's clauses.
        set_clause(&sat_st.formula[clauses], clause_length, clause_buffer);
        
        clauses++;
    }
    
    initialize_list(&sat_st.backtracking_status);
    
    free( buffer );
    free( clause_buffer );
    
}

void print_formula(){
    
    int clause = 0;
    int literal;
    
    while (clause < sat_st.num_clauses){
        
        literal = 0;
        while (literal < sat_st.formula[clause].size){
            
            if (literal){
                printf(" ");
            }
            printf("%d", sat_st.formula[clause].literals[literal]);
            
            literal++;
        }
        printf("\n");
        
        clause++;
    }
}

/*
int decide_next_branch(){
    if ( empty(backtracking_status) ){
        // La asignacion es 1
    } else {
        if ( top(backtracking_status).missing_branch == 1){
            La asignacion es voltear la variable
        } else {
            La asignacion es la siguiente variable no asignada
        }
    }
}

int solver(){
    
    sat_st.status = preprocess();
    
    if ( sat_st.status != UNKNOWN){
        return sat_st.status;
    }
    
    while ( TRUE ){
        sat_st.status = decide_next_branch();
        while ( TRUE ){
            sat_st.status = deduce();
            if ( sat_st.status == CONFLICT ){
                int blevel = analyze_conflict();
                if (blevel == 0){
                    return UNSATISFIABLE;
                } else {
                    while ( backtracking_status.size() != blevel ){
                        // Hacer pop y deshacer cambios
                    }
                }
            } else if (sat_st.status == SATISFIABLE){
                return SATISFIABLE;
            } else {
                break;
            }
        }
    }
}

*/


/* Status:
Si la formula es satisfecha retornar TRUE
Si hay contradiccion retornar FALSE
En caso contrario retornar UNKNOWN
*/
//int deduce(){
//  // Take the decision_level_data at top of the stack backtracking-status
//  // and thus obtain the newly assigned_literal.
//  decision_level_data* dec_level_data
//    = (decision_level_data*) top( &(sat_st.backtracking_status) );
//  list* clauses_made_true;
//  list* clauses_not_made_true;
//
//  variable newly_assigned_lit = dec_level_data->assigned_literal;
//  // Assign this variable the value 'value'.
//  // get value somehow....
//  if ( value == TRUE ) {
//    clauses_made_true     = &(sat_st.pos_occurrence_list[literal]);
//    clauses_not_made_true = &(sat_st.neg_occurrence_list[literal]);
//  }
//  else {
//    clauses_made_true     = &(sat_st.neg_occurrence_list[literal]);
//    clauses_not_made_true = &(sat_st.pos_occurrence_list[literal]);
//  }
//  // El ultimo parametro al assign no tiene nada que ver con asignar, ideal
//  // seria que no sea parametro.
//  // assign( newly_assigned_lit, value, clauses_made_true );
//  // unit_propagation( clauses_not_made_true, dec_level_data, literal);
//  // ...falta el cómo retorna valores el unit_propagation..
//}
//
//@ pre literal>=0
int assign( variable literal, char value, list* clauses_made_true ) {
  literal = abs( literal ); // Tal vez no necesario.
  variable deduced_literal;

  sat_st.model[literal] = value;
 
  set_newly_satisfied_clauses( clauses_made_true );
  
  /// Traverse the list of clauses in which @e literal occurs negatively and
  /// change the clauses' watchers if necessary.
}

/**
 *    
 * Add the newly satisfied clauses (their satisfaction is a consequence
 * of the assignment of the @e literal) to the current
 * decision_level_data.
 * @param clauses_made_true 
 *
 */
void set_newly_satisfied_clauses( list* clauses_made_true ) {
  int i;
  node* current_node = (node*) clauses_made_true->first;
  decision_level_data* dec_level_data
    = (decision_level_data*) top( &(sat_st.backtracking_status) );
    
  for ( i=0; i<clauses_made_true->size; i++ ) {
    ((clause*)current_node->item)->satisfied = TRUE; 

    queue( &(dec_level_data->satisfied_clauses), current_node->item );

    current_node = current_node->next;
  }
}

//variable* unit_propagation( list* clauses_not_made_true,
//                            variable literal) {
// 
//  int i;
//  node* current_node = (node*) clauses_not_made_true->first;
//    
//  for ( i=0; i<clauses_not_made_true->size; i++ ) {
//    if ( is_head_watcher((clause*)current_node->item, literal) ) {
//      update_watcher( (clause*) current_node->item );
//    }
//    else if ( is_tail_watcher((clause*)current_node->item, literal) ) {
//
//    }
//
//    current_node = current_node->next;
//  }
//  
//}
//

/**
 * Returns the value in the model for a literal.
 *
 */
int current_literal_value( variable* literal ) {
  return sat_st.model[abs(*literal)];
}

int main(int argc, char* argv[]){
    set_initial_sat_status(argv[1]);
    print_formula();
    return 0;
}
