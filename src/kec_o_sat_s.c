/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Experimental SAT solver that uses non-chronological backtracking, watched
 * literals, unit progation, and other techniques to solve the given SAT
 * instance.
 *
 * The input to the solver has to be in the
 * @see <a href="http://logic.pdmi.ras.ru/~basolver/dimacs.html">DIMACS</a>
 * format.
 *
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/

#include<stdio.h>
#include"sat.h"
#include"sat_io.h"

void print_usage();

int main(int argc, char* argv[]){

    if(argc<2){
        print_usage();
        exit(1);
    }

    //Parse the arguments, and stor them in the SAT_global_settings
    //global variable sat_gs. The next ones are the default
    //options:
    sat_gs.program_name = argv[0];
    sat_gs.input_file = NULL;
    sat_gs.output_file = NULL;
    sat_gs.verbose_mode = FALSE;
    sat_gs.detect_pure_literals = FALSE;
    
    {
        int i=1;
        while( i<argc ){
            
            if( strcmp(argv[i],"-f") == 0 && i+1<argc ){
                
                sat_gs.input_file = argv[++i];
                
            } else if ( strcmp(argv[i], "-o") == 0 && i+1<argc){
                
                sat_gs.output_file = argv[++i];

            } else if ( strcmp(argv[i], "-v") == 0 ){
                
                sat_gs.verbose_mode = TRUE;

            } else if ( strcmp(argv[i], "-pl") == 0 ){
                
                sat_gs.detect_pure_literals = TRUE;

            }else {
                print_usage();                
                exit(1);
            }
            
            i++;
        }

    }

    //Initialize the instance, print the initial status,
    //and solve
    
    set_initial_sat_status();
    
    int status = solve_sat();
    
    if ( status == SATISFIABLE ){
        printf("%s: SATISFIABLE\n\n",sat_gs.program_name);
    } else {
        printf("%s: UNSATISFIABLE\n\n",sat_gs.program_name);
    }
    
    if ( sat_gs.output_file != NULL){
        print_sol(status);
    }
    
    return 0;
}

void print_usage(){

    printf("usage: %s -f <cnf_file> [OPTIONS]\n",sat_gs.program_name);
    printf("\n");
    printf("    OPTIONS:\n");
    printf("\n");
    printf("    -h                Print this message\n");
    printf("\n");
    printf("    -o <output_file>  Write results in <output_file>\n");
    printf("                      if not provided, the reults will\n");
    printf("                      be writen through the stdout\n");
    printf("\n");
    printf("    -v                Activates verbose mode; Prints detailed\n");
    printf("                      information about the status of the algorithm\n");
    printf("                      when executing\n");
    printf("\n");
    printf("    -pl               Activates pure literal deduction algorithm\n");
    printf("                      for detecting pure literals in each iteration\n");

}


