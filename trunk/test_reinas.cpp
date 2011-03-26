#include <iostream>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <sstream>

using namespace std;

int main(int argc, char* argv[]){
    
    int min, max;
    {
        stringstream ss(argv[1]);
        ss >> min;
    }
    {
        stringstream ss(argv[2]);
        ss >> max;
    }
    
    for (int i=min; i<=max; i++){
        
        cout << "Test: " << i << "-reinas\n";
        cout << "Sin Aprendizaje:\n";
        
        char command[100];
        
        memset(command,0,sizeof(command));
        sprintf(command, "./kec_o_sat_s -l -f test/%dreinas.cnf -t 100", i);
        system(command);
        
        cout << "Con Aprendizaje:\n";
        
        memset(command,0,sizeof(command));
        sprintf(command, "./kec_o_sat_s -f test/%dreinas.cnf -t 100", i);
        system(command);
        
        char aux;
        scanf("%c", &aux);
    }
    
    return 0;
}