#include "sat.h"

void free_decision_level_data(decision_level_data* dld){
    //free_list(dld->propagated_var);
    //free_list(dld->satisfied_clauses);
    free(dld);
}

/**
 * The purpose of this procedure is to add the clause cl to the list of
 * clauses where v is watched positively or negatively.
 *
 * @param v The variable that is watched.
 * @param cl A pointer to the clause where v is watched
 *
 */
inline void add_to_watched_list(variable v, clause* cl){
    
    if ( v > 0 ){
        push(&sat_st.pos_watched_list[abs(v)], cl);
    } else {
        push(&sat_st.neg_watched_list[abs(v)], cl);
    }
}

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
void set_clause( clause* cl, int clause_length, int lit[] ){
    
    cl->size = clause_length;
    
    cl->literals = (variable*) malloc (clause_length*sizeof(variable));
    memcpy( cl->literals, lit, clause_length*sizeof(int) );
    
    cl->head_watcher = cl->literals;
    
    add_to_watched_list(*cl->literals, cl);
    
    /* OJO: en este punto habria que chequear que no aparezca una variable
            varias veces, si no podriamos eliminar ocurrencias o hacer true
            la clausula
     */
    
    if ( clause_length > 1 ){
        cl->tail_watcher = cl->literals + 1;
        
        add_to_watched_list(*(cl->literals + 1), cl);
        
    } else {
        cl->tail_watcher = cl->literals;
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
    
    sat_st.pos_watched_list =
        (list*) malloc( (sat_st.num_vars+1)*sizeof(list) );
    sat_st.neg_watched_list =
        (list*) malloc( (sat_st.num_vars+1)*sizeof(list) );
    
    memset( sat_st.pos_watched_list, 0, (sat_st.num_vars + 1)*sizeof(list));
    memset( sat_st.neg_watched_list, 0, (sat_st.num_vars + 1)*sizeof(list));
    
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
    int empty_clause;
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
    empty_clause = 0;
    
    while ( clauses < sat_st.num_clauses && !empty_clause){
        
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
            
            /*
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
            */
            
            // We search for the next literal in the buffer.
            // A literal corresponds to a number in the buffer.
            while ( isdigit(*l_aux) || *l_aux == '-')
                l_aux++;
            while ( !( isdigit(*l_aux) || *l_aux == '-') )
                l_aux++;
            
            clause_length++;
        }
        
        if ( clause_length == 0 ){
            empty_clause = 1;
        } else {
            // We copy the array of literals just read (clause_buffer) in the
            // current line to one of sat_st's clauses.
            set_clause(&sat_st.formula[clauses], clause_length, clause_buffer);
        }
        
        clauses++;
    }
    
    initialize_list(&sat_st.backtracking_status);
    
    free( buffer );
    free( clause_buffer );
    
    fclose(file);
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


/**
  Choose a next variable, push the info in the backtracking_status
  stack. MAke sure that if the stack is empty is because it's
  the first assignment (else you'll get an infinite loop in the solver).

  This next branch things takes care of flippings! And also
  backtracking-stuff due to this flippings!

  Return TRUE or FALSE if there where more variables to assign
 */
int decide_next_branch(){
    //Create the structure that will be pushed
    decision_level_data *dec_lev_dat =
        (decision_level_data*)malloc(sizeof(decision_level_data));

    //Initialize the lists
    dec_lev_dat->propagated_var = *(new_list()); 
    dec_lev_dat->satisfied_clauses = *(new_list()); 

    //Choose the next unassigned variable. TODO do this more efficient!

    variable free_variable=1;
    while(  free_variable <= sat_st.num_vars &&
            sat_st.model[free_variable] != UNKNOWN
        ){
            free_variable++;
    }
    
    //If there are no more variables to assign, report an error
    if( free_variable > sat_st.num_vars ){
        push((&sat_st.backtracking_status), (void*)NULL);
        fprintf(stderr,"ERROR! Trying to assign a new variable, but no");
        fprintf(stderr," free variables are available!\n");
        exit(1);
    }
    dec_lev_dat->assigned_literal = free_variable;
    dec_lev_dat->missing_branch = TRUE;
    
    //Push the structure in the stack
    push((&sat_st.backtracking_status), dec_lev_dat);
    
    return 1;
}


/**
 * This function tryes to solve the sat_instance that is stored
 * in the global variable sat_st.
 *
 * @return   SATISFIABLE if an assignment to the SAT variables is
 *           found that satisfies the formula. Otherwise, UNSATISFIABLE.
 */
int solve_sat(){
    
    //TODO make a preprocess

    //The algorithm is an iterative backtracking.

    //Decide next branch, and push it as a
    //decision_level_data structure in the
    //backtracking_status variable of the global
    //sat_st variable.
    decide_next_branch();

    while ( TRUE ){

        //Check the top variable in the stack, if the
        //stack is empty, then all possible assignments were tryed
        //and the formula is UNSATISFIABLE
        if( empty(&sat_st.backtracking_status) )
            return UNSATISFIABLE;

        decision_level_data *top_el =
            (decision_level_data*) top(&sat_st.backtracking_status);

        //Make the assignment of the literal that appeared on top
        //of the stack
        printf("solve_sat: deducing -> %d\n",top_el->assigned_literal);
        int assignment_result = deduce(top_el->assigned_literal);
        printf("solve_sat: result of %d -> ",top_el->assigned_literal);

        //If the result is UNKNOWN, continue the recursion (iteratively)
        if( assignment_result == DONT_CARE ){
            printf("dont care\n");
            decide_next_branch();
            continue;
        }
    
        //If the assignment satisfied the formula, return a positive
        //answer and finish the funtion
        else if( assignment_result == UNIT_CLAUSE ){  //TODO this should be SATISFIED
            printf("SATISFIED!\n");
            return SATISFIABLE;
        }
        //If the assignment made the formula FALSE, then backtrack until
        //a variable that can be flipped is found.
        else if( assignment_result == CONFLICT ){ 
            printf("CONFLICT!\n");

            decision_level_data *top_el = NULL;

            do {

                top_el = (decision_level_data*) 
                    top(&sat_st.backtracking_status);

                //Undo the assignments made in the
                //decision level
                undo_assignments(top_el);

                if( top_el->missing_branch == TRUE ){
                    //If the opposite branch has not been tryed,
                    //then flip the assignment and continue the
                    //recursion (iteratively)

                    //Flip the value
                    top_el->assigned_literal =
                        - top_el->assigned_literal; 

                    top_el->missing_branch = FALSE;
                    break;

                } else {
                    //Destroy the structure and continue
                    //the search for a variable that can be flipped
                    free_decision_level_data(top_el);
                    pop(&sat_st.backtracking_status);
                }

            } while( !empty(&sat_st.backtracking_status) );
            
            //If the stack got empty, then no backtracking was
            //possible, hence, the formula is UNSATISFIABLE
            if( empty(&sat_st.backtracking_status) )
                return UNSATISFIABLE;
        }

    }

}

void undo_assignments(decision_level_data *dec_lev_dat){
    //First, unset the assigned variable in the
    //given decision level
    variable assigned_var = abs(dec_lev_dat->assigned_literal);

    sat_st.model[ assigned_var ] = UNKNOWN;
    printf("Undoing variable: %d\n",assigned_var);

    //Similarly, unset the assignments for the propageted
    //literals
    while( !empty(&dec_lev_dat->propagated_var) ){

        variable *watcher =
            (variable*)top(&dec_lev_dat->propagated_var);

        assigned_var = abs(*watcher);
        sat_st.model[ assigned_var ] = UNKNOWN;
        printf("Undoing variable: %d\n",assigned_var);

        pop(&dec_lev_dat->propagated_var);

    }

}

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

/**
 *
 * This function receives a @e literal and performs Boolean Constrain
 * Propagation (BCP) by calling the appropriate functions that perform Unit
 * Clause Propagation and Pure Literal Elimination.
 *
 * @param literal The variable selected for assignment by the function
 *        @e decide_next_branch.
 * @return Returns a status that indicates how the assignment to variable
 *        @e literal went on. It will return SATISFIED, if after assigning
 *        @e literal a truth value and after Unit Clause propagation and
 *        pure literal elimination has been performed, the formula was
 *        found to be satisfied. It will return DONT_CARE(0), if after assigning
 *        @e literal a truth value and after Unit Clause propagation and
 *        pure literal elimination has been performed, the formula hasn't
 *        still been satisfied and there is still no conflict, so there's the
 *        chance that if we continue to assign some other variables we may find
 *        the formula to be satisfied or conflicted. It will return CONFLICT(2),
 *        if the assignment of @e literal with some value, along with the values
 *        that were previously assigned to some variables is conflictive.
 *        
 */
int deduce( variable literal ) {
    // A variable is a signed integer. If the sign is '-' the variable
    // abs(literal) was assigned a false value. If the sign is '+', it was
    // assigned a truth value.
    variable abs_literal = abs( literal );
    
    list* clauses_affected;
    
    sat_st.model[abs_literal] = (abs_literal/literal == 1 ? TRUE : FALSE);
    
    if ( sat_st.model[abs_literal] == TRUE ) {
        clauses_affected = &(sat_st.neg_watched_list[abs_literal]);
    } else {
        clauses_affected = &(sat_st.pos_watched_list[abs_literal]);
    }
    
    
    if ( clauses_affected->size > 0 ) {
        return set_newly_watchers( clauses_affected, literal );        
    }
    
    return DONT_CARE;
}

int set_newly_watchers( list* clauses_affected, variable literal )
{
    
    stack unit_clauses;
    initialize_list(&unit_clauses);
    
    int status = DONT_CARE;
    
    while( !empty(clauses_affected) && status == DONT_CARE ) {
        
        clause* cl = ((node*) clauses_affected->first)->item;
        
        if ( is_head_watcher(cl, literal) ) {
            status = update_watcher( cl );
        }
        else if ( is_tail_watcher(cl, literal) ) {
            swap_watchers( cl );
            status = update_watcher( cl );
        }
        
        // We maintain a stack of unitary clauses which need further
        // propagation. (Since they are unitary we need to properly assign their
        // single variables). We save them for a little later.
        if (status == UNIT_CLAUSE){
          // @Assert: cl->tail_watcher points to the single variable of the
          // unitary clause.
            printf("clausula %d es unitaria\n", cl - sat_st.formula);
            push( &unit_clauses, cl );
            status = DONT_CARE;
        } else {
            pop(clauses_affected);
            add_to_watched_list(*cl->head_watcher, cl);
        }
    }
    
    //@Assert: status== CONFLICT || status == DONT_CARE.
    if (status == DONT_CARE){
        return unit_propagation( &unit_clauses );
    } else {
        //@Assert: status == CONFLICT.
        printf("cambio de estado a %d\n", status);
        while( !empty(&unit_clauses) ){
            pop( &unit_clauses );
        }
        
        return status;
    }
}

/**
 * This function receives a stack of unitary clauses whose single variables need
 * to be properly assigned and propagated.
 *    
 * @param unit_clauses A stack of unitary clauses whose single variables need to
 *        be properly assigned and propagated.
 * @return UNIT_CLAUSE(1) If the clause head_clause is unitary.
 *         CONFLICT(2)    If the clause is conflictive with the current model.
 *         DONT_CARE(0)   Neither of the previous two alternatives.
 * @pre unit_clauses != NULL;
 */

int unit_propagation( stack* unit_clauses )
{
    
    int status = DONT_CARE;
    decision_level_data* dec_level_data
            = (decision_level_data*) top( &(sat_st.backtracking_status) );
    
    while( !empty(unit_clauses ) && status == DONT_CARE){
        
        clause* cl = (clause*)top(unit_clauses);
        // We need to keep track of the variables that were propagated, to
        // ensure the correctness of the backtracking procedure.
        if ( dec_level_data ){
            queue( &(dec_level_data->propagated_var), cl->tail_watcher );
        }
        
        // Propagate the single variable in each the unitary clause. The
        // tail_watcher points to its single variable.
        status = deduce( *cl->tail_watcher );
        
        pop(unit_clauses);
    }
    
    while( !empty(unit_clauses ) ){
        pop(unit_clauses);
    }
    
    return status;
}

/**
 * Returns the value in the model for a literal. If this literal hasn't still
 * been assigned it returns UNKNOWN(-1).
 * @param literal A pointer to the variable about which we would like to know
 *        its value.
 * @return TRUE(1)     if the value has been assigned TRUE.
 *         FALSE(0)    if the value has been assigned FALSE.
 *         UNKNOWN(-1) if the value has not been assigned.
 */
int current_literal_value( variable* literal ) {
  return sat_st.model[abs(*literal)];
}

/**
 * 
 * @param cl A pointer to a clause.
 * @param literal A variable.
 * @return TRUE(1)     if the variable @e literal is the head watcher of @e cl.
 *         FALSE(0)    if the variable @e literal isnt' the head watcher of @e
 *                     cl. 
 */

int is_head_watcher( clause* cl, variable literal ){
    return abs(*(cl->head_watcher)) == abs(literal);
}

/**
 * 
 * @param cl A pointer to a clause.
 * @param literal A variable.
 * @return TRUE(1)     if the variable @e literal is the tail watcher of @e cl.
 *         FALSE(0)    if the variable @e literal isnt' the tail watcher of @e 
 *                     cl.
 */

int is_tail_watcher( clause* cl, variable literal ){
    return abs(*(cl->tail_watcher)) == abs(literal);
}

/**
 * This functions swaps the head_watcher and the tail_watcher of the clause cl.
 *
 * @param cl A clause parameter
 * @pre cl != NULL;
 *
 */
void swap_watchers( clause* cl ){
    variable* tmp = cl->head_watcher;
    cl->head_watcher = cl->tail_watcher;
    cl->tail_watcher = tmp;
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

/**
 * Given a clause and a freshly recently assigned variable occurring in the
 * aforementioned clause that is being pointed to by the head_watcher, searches
 * for some other unassigned literal to be the new head_watcher. 
 * This method determines if a clause a unitary clause or a conflictive clause.
 *
 * @param head_clause
 * @return UNIT_CLAUSE(1) If the clause head_clause is unitary.
 *         CONFLICT(2)    If the clause is conflictive with the current model.
 *         DONT_CARE(0)   Neither of the previous two alternatives.
 * @pre head_clause != NULL && head_clause->satisfied != TRUE;
 *
 */

int update_watcher( clause* head_clause ) {
    
    printf("%d\n", *head_clause->head_watcher);
    printf("%d\n", *head_clause->tail_watcher);
    printf("clause %d (%d,%d) ->", head_clause - sat_st.formula,
                                  *head_clause->head_watcher,
                                  *head_clause->tail_watcher);
    
    if ( is_satisfied( *head_clause->head_watcher ) 
           || is_satisfied( *head_clause->tail_watcher ) )
    {
        return DONT_CARE;
    }
    
    variable current_literal;
    
    // This variable will indicate if in the clause @head_clause there's an
    // unassigned literal.
    int exists_free_literal = FALSE;    
    {
        int i;
        // Iterate over the clause head_clause searching for an unassigned
        // literal not pointed to by the tail_watcher. If it finds one such
        // unassigned literal, make the head_watcher point to it.
        for ( i=0; i < head_clause->size; i++ ){
            current_literal = abs( head_clause->literals[i] );
            
            if ( is_satisfied( head_clause->literals[i]) ){
                exists_free_literal = TRUE;
            }
            
            if ( sat_st.model[current_literal] == UNKNOWN
                && head_clause->tail_watcher != head_clause->literals + i )
            {
                // @Assert: There's an unassigned literal not pointed to by the
                // tail_watcher.
                head_clause->head_watcher = (head_clause->literals) + i;
                exists_free_literal = TRUE;
                break;
            }
        }
    }
    
    printf(" (%d,%d)\n", *head_clause->head_watcher,
                         *head_clause->tail_watcher);
    
    if ( !exists_free_literal ) {
        // @Assert: Either all literals are assigned or the only unassigned
        // literal is pointed to by the tail_watcher.
        
        // assign( *(head_clause->tail_watcher), TRUE );
        
        if ( current_literal_value(head_clause->tail_watcher) == UNKNOWN ) {
          // @Assert: This clause is a unit clause.
            return UNIT_CLAUSE;
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
    
    //int status = deduce(725);
    int status = solve_sat();
    
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

