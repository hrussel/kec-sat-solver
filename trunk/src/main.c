#include "sat.h"

int main(int argc, char* argv[]){

    if(argc<2){
        printf("usage: %s -f <cnf_file> [OPTIONS]\n",argv[0]);
        exit(1);
    }

    //Parse the arguments, the next ones are the default
    //options
    char *cnf_file = NULL;

    {
        int i=1;
        while( i<argc ){
            
            if( strcmp(argv[i],"-f") == 0 && i+1<argc ){

                cnf_file = argv[++i];

            } else {

                printf("usage: %s -f <cnf_file> [OPTIONS]\n",argv[0]);
                printf("    OPTIONS:\n");
                printf("    -h      Print this message\n");
                exit(1);

            }

            i++;
        }

    }

    //Initialize the instance, print the initial status,
    //and solve

    set_initial_sat_status(cnf_file);
    
    print_status();
    int status = solve_sat();
    
    print_status();
    printf("status %d\n", status);
    
    print_sol(status, argv[2]);
    return 0;
}
