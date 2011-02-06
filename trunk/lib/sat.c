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
        cl->tail_watcher = cl->literals;
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
    sat_st.pos_occurrence_list =
        (list*) malloc( (sat_st.num_vars+1)*sizeof(list) );
    sat_st.neg_occurrence_list =
        (list*) malloc( (sat_st.num_vars+1)*sizeof(list) );
    
    memset( sat_st.pos_occurrence_list, 0, (sat_st.num_vars + 1)*sizeof(list));
    memset( sat_st.neg_occurrence_list, 0, (sat_st.num_vars + 1)*sizeof(list));
    
    // Allocate space for the model: the current assignment of truth values
    // to literals that is being studied.
    sat_st.model = (int*)malloc( (sat_st.num_vars+1)*sizeof(int) );
    memset(sat_st.model, -1, (sat_st.num_vars+1)*sizeof(int));
    
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
    int clauses, current_literal, clause_length;
    int *clause_buffer;
    FILE * file;
    size_t nbytes;
    
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
        int r_getline;
        
        r_getline = getline (&buffer, &nbytes, file);
        
        if ( r_getline == 0){
            printf("Error\n");
            exit(1);
        }
        
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
    
    clause = 0;
    while (clause < sat_st.num_clauses){
        printf("%d ", sat_st.formula[clause].satisfied);
        clause++;
    }
    printf("\n");
    
    literal = 1;
    while(literal <= sat_st.num_vars){
        printf("(%d:%d) ", literal, sat_st.model[literal]);
        literal++;
    }
    printf("\n");
    
    literal = 0;
    clause = 0;
    while (clause < sat_st.num_clauses){
        
        if ( !sat_st.formula[clause].satisfied ){
            
            literal = 0;
            int first = 1;
            
            while (literal < sat_st.formula[clause].size){
                
                int variable = sat_st.formula[clause].literals[literal];
                int abs_variable = abs(variable);
                
                if ( sat_st.model[abs_variable] != UNKNOWN )
                {
                    if (( sat_st.model[abs_variable] == TRUE
                            && variable < 0)
                            || ( sat_st.model[abs_variable] == FALSE
                                 && variable > 0)
                        )
                    {
                        literal++;
                        continue;
                    }
                }
                
                if (!first){
                    printf(" ");
                }
                first = 0;
                
                printf("%d", sat_st.formula[clause].literals[literal]);
                        
                literal++;
            }
            
            printf("\n");
            
        }
        
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
        
        <-- OJO --> al hacer la asignacion se hace automaticamente la deduccion
                    NO ES NECESARIO EL CICLO, SOLO VERIFICAR EL RESULTADO DE LA
                    ASIGNACION
        
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

int deduce( variable literal ) {
    
    variable abs_literal = abs( literal );
    
    list* clauses_made_true;
    list* clauses_not_made_true;
    
    sat_st.model[abs_literal] = (abs_literal/literal == 1 ? TRUE : FALSE);
    
    if ( sat_st.model[abs_literal] == TRUE ) {
        clauses_made_true     = &(sat_st.pos_occurrence_list[abs_literal]);
        clauses_not_made_true = &(sat_st.neg_occurrence_list[abs_literal]);
    }
    else {
        clauses_made_true     = &(sat_st.neg_occurrence_list[abs_literal]);
        clauses_not_made_true = &(sat_st.pos_occurrence_list[abs_literal]);
    }
    
    set_newly_satisfied_clauses( clauses_made_true );
    
    if ( clauses_not_made_true->size > 0 ) {
        
        return set_newly_unsatisfied_clauses( clauses_not_made_true, literal );
    }
    
    return DONT_CARE;
    
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
        
        if (dec_level_data){
            
            queue( &(dec_level_data->satisfied_clauses), current_node->item );
        }
        
        current_node = current_node->next;
    }
}

int set_newly_unsatisfied_clauses( list* clauses_not_made_true,
                                    variable literal )
{
    int i;
    node* current_node = (node*) clauses_not_made_true->first;
    
    stack unit_clauses;
    initialize_list(&unit_clauses);
    
    int status = DONT_CARE;
    
    for ( i=0; i<clauses_not_made_true->size && status == DONT_CARE; i++ ) {
        
        clause* cl = (clause*)current_node->item;
        
        if ( cl->satisfied ){
            continue;
        }
        
        if ( is_head_watcher(cl, literal) ) {
            status = update_watcher( cl );
        }
        else if ( is_tail_watcher(cl, literal) ) {
            swap_watchers( cl );
            status = update_watcher( cl );
        }
        
        if (status == UNIT_CLAUSE){
            printf("clausula %d es unitaria\n", cl - sat_st.formula);
            push(&unit_clauses, cl);
            status = DONT_CARE;
        }
        
        current_node = current_node->next;
    }
    
    if (status == DONT_CARE){
        return unit_propagation( &unit_clauses);
    } else {
        printf("cambio de estado a %d\n", status);
        while(!empty(&unit_clauses)){
            pop(&unit_clauses);
        }
        
        return status;
    }
}

int unit_propagation( stack* unit_clauses)
{
    
    int status = DONT_CARE;
    decision_level_data* dec_level_data
            = (decision_level_data*) top( &(sat_st.backtracking_status) );
    
    while( !empty(unit_clauses ) && status == DONT_CARE){
        
        clause* cl = (clause*)top(unit_clauses);
        
        if ( dec_level_data ){
            queue( &(dec_level_data->propagated_var), cl->tail_watcher );
        }
        
        status = deduce( *cl->tail_watcher );
        
        pop(unit_clauses);
    }
    
    while( !empty(unit_clauses ) ){
        pop(unit_clauses);
    }
    
    return status;
}

/**
 * Returns the value in the model for a literal.
 *
 */
int current_literal_value( variable* literal ) {
  return sat_st.model[abs(*literal)];
}

int is_head_watcher(clause* cl, variable literal){
    return abs(*(cl->head_watcher)) == abs(literal);
}

int is_tail_watcher(clause* cl, variable literal){
    return abs(*(cl->tail_watcher)) == abs(literal);
}

void swap_watchers(clause* cl ){
    variable* tmp = cl->head_watcher;
    cl->head_watcher = cl->tail_watcher;
    cl->tail_watcher = tmp;
}

int update_watcher( clause* head_clause ) {
    
    printf("%d\n", *head_clause->head_watcher);
    printf("%d\n", *head_clause->tail_watcher);
    printf("clause %d (%d,%d) ->", head_clause - sat_st.formula,
                                  *head_clause->head_watcher,
                                  *head_clause->tail_watcher);
    
    variable current_literal;
    
    int exists_free_literal = FALSE;
    
    {
        int i;
        for ( i=0; i < head_clause->size; i++ ){
            current_literal = abs( head_clause->literals[i] );
            
            if ( sat_st.model[current_literal] == UNKNOWN
                && head_clause->tail_watcher != head_clause->literals + i )
            {
                head_clause->head_watcher = (head_clause->literals) + i;
                exists_free_literal = TRUE;
                break;
            }
        }
    }
    
    printf(" (%d,%d)\n", *head_clause->head_watcher,
                         *head_clause->tail_watcher);
    
    if ( !exists_free_literal ) {
        
        // The only literal not set to 0 in the clause is the other
        // watcher.
        // This clause is a unit clause.
        // assign( *(head_clause->tail_watcher), TRUE );
        
        if ( current_literal_value(head_clause->tail_watcher) == UNKNOWN ) {
            // This clause is not satisfiable.
            return UNIT_CLAUSE;
        }
        
        return CONFLICT;
    }
    
    return DONT_CARE;
}


int main(int argc, char* argv[]){
    set_initial_sat_status(argv[1]);
    
    /* BUEN EJEMPLO PARA cnf_small.cnf
    
    print_formula();
    printf("\nasignar: -1\n");
    assign(-1);
    
    print_formula();
    printf("\n");
    
    */
    
    print_formula();
    
    int status = deduce(725);
    
    print_formula();
    printf("\n");
    printf("%d\n", status);
    
    /*
    printf("\nasignar: 1\n");
    assign(1);
    print_formula();
    printf("\nasignar: -4\n");
    assign(-4);
    print_formula();
    */
    return 0;
}

