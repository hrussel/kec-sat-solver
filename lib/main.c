#include "sat.h"

int main(int argc, char* argv[]){
    set_initial_sat_status(argv[1]);
    
    int status = solve_sat();
    
    printf("status %d\n", status);
    
    print_sol(status, argv[2]);
    return 0;
}
