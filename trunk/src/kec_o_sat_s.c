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

int main(int argc, char* argv[]){

    if(argc<2){
        printf("usage: %s -f <cnf_file> [OPTIONS]\n",argv[0]);
        exit(1);
    }

    //Parse the arguments, the next ones are the default
    //options
    char *cnf_file = NULL;
    char *output_file = NULL;
    
    {
        int i=1;
        while( i<argc ){
            
            if( strcmp(argv[i],"-f") == 0 && i+1<argc ){
                
                cnf_file = argv[++i];
                
            } else if ( strcmp(argv[i], "-o") == 0 && i+1<argc){
                
                output_file = argv[++i];
                
            }else {
                
                printf("usage: %s -f <cnf_file> [OPTIONS]\n",argv[0]);
                printf("    OPTIONS:\n");
                printf("    -h                Print this message\n");
                printf("    -o <output_file>  Write results in <output_file>\n");
                exit(1);
                
            }
            
            i++;
        }

    }

    //Initialize the instance, print the initial status,
    //and solve
    
    set_initial_sat_status(cnf_file);
    
    int status = solve_sat();
    
    if ( status == SATISFIABLE ){
        printf("kec_o_sat_s: SATISFIABLE\n\n");
    } else {
        printf("kec_o_sat_s: UNSATISFIABLE\n\n");
    }
    
    if ( output_file != NULL){
        print_sol(status, output_file);
    }
    
    return 0;
}
