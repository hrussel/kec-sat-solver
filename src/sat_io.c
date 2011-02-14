/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Source code for the procedures that intialize the sat instance by reading
 * a provided file in the 
 * @see <a href="http://logic.pdmi.ras.ru/~basolver/dimacs.html">DIMACS</a>
 * format, and also writing solution thorugh output.
 * 
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/

#include "sat_io.h"

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
        cl->tail_watcher = cl->head_watcher;
    }
}

void allocate_sat_status(){
    // Allocate space for the boolean formula.
    sat_st.formula = (clause*) malloc ( sat_st.num_clauses*sizeof(clause) );
    if( sat_st.formula == NULL )
        report_io_error(3);
    
    sat_st.pos_watched_list =
        (list*) malloc( (sat_st.num_vars+1)*sizeof(list) );
    if( sat_st.pos_watched_list == NULL )
        report_io_error(3);

    sat_st.neg_watched_list =
        (list*) malloc( (sat_st.num_vars+1)*sizeof(list) );
    if( sat_st.neg_watched_list == NULL )
        report_io_error(3);

    memset( sat_st.pos_watched_list, 0, (sat_st.num_vars + 1)*sizeof(list));
    memset( sat_st.neg_watched_list, 0, (sat_st.num_vars + 1)*sizeof(list));
    
    // Allocate space for the model: the current assignment of truth values
    // to literals that is being studied.
    sat_st.model = (int*)malloc( (sat_st.num_vars+1)*sizeof(int) );
    if( sat_st.model == NULL )
        report_io_error(3);

    memset(sat_st.model, -1, (sat_st.num_vars+1)*sizeof(int));
    
}

void set_initial_sat_status(){
    
    char *buffer;
    int clauses, current_literal, clause_length;
    int *clause_buffer;
    int empty_clause;
    FILE * file;
    size_t nbytes;
    
    file = fopen (sat_gs.input_file,"r");
    if ( file == NULL ){
        report_io_error(1);
    }
    
    buffer = (char*) malloc( (BUFFERSIZE + 1)*sizeof(char) );    
    if ( buffer == NULL ){
        report_io_error(3);
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
        
        if ( r_getline <= 0){
            report_io_error(4);
        }
        
        if (*buffer == 'c')
            continue;
        
        l_aux = buffer;
        
        clause_length = 0;
        
        while ( sscanf(l_aux, "%d", &current_literal) && current_literal != 0 )
        {
            
            clause_buffer[clause_length] = current_literal;
            
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
    
    literal = 0;
    clause = 0;
    while (clause < sat_st.num_clauses){
        
        int satisfied = FALSE;
        
        {
            int k;
            for (k=0; k<sat_st.formula[clause].size && !satisfied; k++){
                satisfied = is_satisfied(sat_st.formula[clause].literals[k]);
            }
        }
        
        printf("    (%d)", clause);
        
        if ( !satisfied ){
            
            literal = 0;
            
            while (literal < sat_st.formula[clause].size){
                
                int variable = sat_st.formula[clause].literals[literal];
                int abs_variable = abs(variable);
                
                if ( sat_st.model[abs_variable] == UNKNOWN )
                {
                    printf(" %d", sat_st.formula[clause].literals[literal]);
                }
                literal++;
            }
        } else {
            printf(" SATISFIED");
        }
        printf("\n");
        
        clause++;
    }
}

void print_status(){
    printf("Assignment:\n");
    
    int i;
    for (i=1; i<=sat_st.num_vars; i++){
        printf("    (%d, %d)\n", i, sat_st.model[i]);
    }
    printf("\n");
    
    printf("Watchers:\n");
    for (i=0; i<sat_st.num_clauses; i++){
        printf("    (%d) (%d, %d)\n", i, *(sat_st.formula[i].head_watcher),
                                      *(sat_st.formula[i].tail_watcher));
    }
    printf("\n");
    
    printf("Formula:\n");
    print_formula();
    
    printf("---------------------------------------------------------------\n");
}

void print_sol(int status){
    
    FILE * file;
    char* buffer;
    
    file = fopen (sat_gs.output_file,"w");
    if ( file == NULL ){
        report_io_error(2);
    }
    
    buffer = (char*) malloc( (BUFFERSIZE + 1)*sizeof(char) );    
    if ( buffer == NULL ){
        report_io_error(3);
    }
    
    if ( status != SATISFIED ){
        fprintf(file, "UNSAT\n");
    } else {
        
        fprintf(file, "1 0\n");
        int i;
        for (i=1; i<=sat_st.num_vars; i++){
            fprintf(file, "%d\n",sat_st.model[i]);
        }
    }
    
    fclose(file);
}

void report_io_error(int kecosats_errno){
    
    char err_mssg[BUFFERSIZE];
    sprintf(err_mssg, "%s ERROR ", sat_gs.program_name);

    switch(kecosats_errno){
        case 1:
            strcat(err_mssg,"while opening file ");
            strcat(err_mssg,sat_gs.input_file);
            break;
        case 2:
            strcat(err_mssg,"while opening file ");
            strcat(err_mssg,sat_gs.output_file);
            break;
        case 3:
            strcat(err_mssg,"allocating memory ");
            break;
        case 4:
            strcat(err_mssg,"parsing input file ");
            strcat(err_mssg,sat_gs.input_file);
            strcat(err_mssg,", bad format.");
            break;
        default:
            strcat(err_mssg,"unknown internal error");
    }
    perror(err_mssg);
    exit(kecosats_errno);
}
