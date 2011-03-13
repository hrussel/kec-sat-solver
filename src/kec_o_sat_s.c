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
#include<signal.h>
#include<unistd.h>
#include <time.h>
#include <sys/time.h>
#include"kecosats_structures.h"
#include"kecosats_algorithm.h"
#include"sat_io.h"

void print_usage();
void catch_alarm();

int main(int argc, char* argv[]){

    //Parse the arguments, and store them in the SAT_global_settings
    //global variable sat_gs. The next ones are the default
    //options:
    sat_gs.program_name = argv[0];
    sat_gs.input_file = NULL;
    sat_gs.output_file = NULL;
    sat_gs.verbose_mode = FALSE;
    sat_gs.time_out = 0;
    sat_gs.detect_pure_literals = FALSE;
    sat_gs.print_statistics = TRUE;
    sat_gs.learn = TRUE;

    if(argc<2){
        print_usage();
        exit(1);
    }

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

            } else if( strcmp(argv[i],"-t") == 0 && i+1<argc ){
                
                sat_gs.time_out = atoi(argv[++i]);
            } else if ( strcmp(argv[i],"-l") == 0 ){
                
                sat_gs.learn = FALSE;
            } else if ( strcmp(argv[i], "-s") == 0 ){
                
                sat_gs.print_statistics = FALSE;
            }else {
                print_usage();                
                exit(1);
            }
            
            i++;
        }

    }
    
    double t_inicial, t_final;
    struct timeval t_p; 
    
    if (!gettimeofday(&t_p,NULL))
       t_inicial = (double) t_p.tv_sec + ((double) t_p.tv_usec)/1000000.0;
    else {
        printf("Error: Bad time request.\n");
        exit(2);
    }
    
    //Set the catcher for alarm signals
    signal(SIGALRM, catch_alarm);

    //Initialize the instance, print the initial status,
    //and solve
    set_initial_sat_status();
    
    //Set the alarm before trying to solve the SAT instance,
    //and unset it after solving
    alarm(sat_gs.time_out);
    int status = solve_sat();
    alarm(0);
    
    if (!gettimeofday(&t_p,NULL))
        t_final = (double) t_p.tv_sec + ((double) t_p.tv_usec)/1000000.0;
    else {
        printf("Error: Bad time request.\n");
        exit(2);
    }
    
    if ( status == SATISFIABLE ){
        printf("\n%s: SATISFIABLE\n\n",sat_gs.program_name);
    } else {
        //print_status();
        printf("\n%s: UNSATISFIABLE\n\n",sat_gs.program_name);
    }
    //print_status();
    
    if ( sat_gs.output_file != NULL){
        print_sol(status);
    }
    
    if ( sat_gs.print_statistics ){
        
        printf("Original clauses:           %d\n",
                    sat_st.num_original_clauses);
        printf("Original variables:         %d\n", sat_st.num_vars);
        printf("\n");
        printf("Time:                       %1.4lf\n", t_final-t_inicial);
        printf("Expanded nodes:             %ld\n", sat_gs.num_expanded_nodes);
        printf("Conflicts:                  %ld\n", sat_gs.num_conflicts);
        printf("Non-chronological jumps:    %ld\n",
               sat_gs.num_non_chronological_jumps);
        printf("Learned clauses:            %ld\n",
                    sat_st.num_clauses + sat_gs.unit_learned_clauses
                    - sat_st.num_original_clauses);
        printf("\n");
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
    printf("\n");
    printf("    -t <time_out>     Abort the execution of the program after 'time_out'\n");
    printf("                      seconds have elapsed\n");
    printf("\n");
    printf("    -l                Deactivates the learning algorithms.\n");
    printf("\n");
    printf("    -s                Deactivates the statistics analysis.\n");
    printf("\n");
}

void catch_alarm(int sig_num){
    report_io_error(5);
}
