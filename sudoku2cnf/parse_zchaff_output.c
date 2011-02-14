#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]){
    
    int num_vars;
    sscanf(argv[1], "%d", &num_vars);
    
    FILE* fin;
    FILE* fout;
    
    fin = fopen(argv[2], "r");
    fout = fopen(argv[3], "w");
    
    if ( fin == NULL || fout == NULL){
        printf("Error: can't open file.\n");
        exit(1);
    }
    
    char buff[10000];
    memset(buff, 0, sizeof(buff));
    
    while( fscanf(fin, "%s", buff) && strcmp(buff, "Instance")){
        memset(buff, 0, sizeof(buff));
    }
    fscanf(fin, "%s", buff);
    
    if ( strcmp(buff, "Satisfiable") == 0 ){
        fprintf(fout, "1 0\n");
        
        while( num_vars--){
            int var;
            
            fscanf(fin, "%d", &var);
            
            if (var > 0){
                fprintf(fout, "1\n");
            } else {
                fprintf(fout, "0\n");
            }
        }
        
    } else {
        fprintf(fout, "UNSAT\n");
    }
}