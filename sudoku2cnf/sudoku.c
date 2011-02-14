#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFERSIZE 1000000
#define VARIABLE(i,j,k) n2*(n2*i + j) + k
#define ROW(c,i) (n * ( c / n ) + (i / n))
#define COLUMN(c,i) (n * (c % n) + (i % n))

/* This function prints a frame line of a sudoku
 */
void print_line(int n){
    
    int i, n2;
    
    n2 = n*n;
    
    printf("+-");
    for (i=1; i<n2; i++){
        if ( i % n == 0 ){
            printf("+-");
        } else {
            printf("---");
        }
    }
    printf("+\n");
}

/* This function prints a sudoku of size n */
void print_sudoku(int** t, int n){
    
    int i, j;
    int n2;
    
    n2 = n*n;
    
    for (i=0; i<n2; i++){
        
        if ( i % n == 0){
            print_line(n);
        }
        
        for (j=0; j<n2; j++){
            if ( j % n == 0 ){
                printf("| ");
            }
            printf("%d ", t[i][j]);
        }
        printf("|\n");
    }
    print_line(n);
    printf("\n");
}

/* It generates a CNF formula corresponding
 * to the sudoku board of size n contained
 * in t.
 * 
 * The instance will be written in the file
 * "filename"
 */
void sudoku2cnf(int** t, int n, char* filename){
    
    int i, j, k, i1, i2, j1, j2, k1, k2, variables, clauses, n2;
    int square, cell1, cell2, asig;
    FILE *f;
    
    n2 = n*n;
    
    asig = 0;
    for (i=0; i<n2; i++){
        for (j=0; j<n2; j++){
            if (t[i][j] != 0){
                asig++;
            }
        }
    }
    
    variables = n2*n2*n2;
    clauses  = n2*n2*(1 + 4*((n2*(n2-1))/2)) + asig;
    
    f = fopen(filename, "w+");
    
    fprintf(f, "p cnf %d %d\n", variables, clauses);
    
    // A value can occur at most once in a cell
    
    for (i = 0; i<n2; i++){
        for (j = 0; j<n2; j++){
            for (k = 1; k<=n2; k++){
                fprintf(f, "%d ", VARIABLE(i, j, k));
            }
            fprintf(f, "0\n");
        }
    }
    
    // A cell can't be assigned to more than one value
    
    for (i = 0; i<n2; i++){
        for (j = 0; j<n2; j++){
            for (k1 = 1; k1 <= n2; k1++){
                for (k2 = 1; k2 < k1; k2++){
                    fprintf(f, "-%d -%d 0\n", VARIABLE(i, j, k1),
                                              VARIABLE(i, j, k2));
                }
            }
        }
    }
    
    // Two cells in a same row, can't be assigned the same value
    for (i = 0; i<n2; i++){
        for (j = 1; j<=n2; j++){
            for (k1 = 0; k1 < n2; k1++){
                for (k2 = 0; k2 < k1; k2++){
                    fprintf(f, "-%d -%d 0\n", VARIABLE(i, k1, j),
                                              VARIABLE(i, k2, j));
                }
            }
        }
    }
    
    // Two cells in a same column, can't be assigned the same value
    for (i = 0; i<n2; i++){
        for (j = 1; j<=n2; j++){
            for (k1 = 0; k1 < n2; k1++){
                for (k2 = 0; k2 < k1; k2++){
                    fprintf(f, "-%d -%d 0\n", VARIABLE(k1, i, j),
                                              VARIABLE(k2, i, j));
                }
            }
        }
    }
    
    // Two cells in a same square, can't be assigned the same value
    for (square = 0; square < n2; square++){
        for (cell1 = 0; cell1 < n2; cell1++){
            for (cell2 = 0; cell2 < cell1; cell2++){
                for (k = 1; k <= n2; k++){
                    fprintf(f, "-%d -%d 0\n",
                               VARIABLE(ROW(square, cell1),
                                        COLUMN(square, cell1),
                                        k),
                               VARIABLE(ROW(square, cell2),
                                        COLUMN(square, cell2),
                                        k)
                            );
                }
            }
        }
    }
    
    // Set the initial state
    for (i=0; i<n2; i++){
        for (j=0; j<n2; j++){
            if (t[i][j] != 0){
                fprintf(f, "%d 0\n", VARIABLE(i, j, t[i][j]));
            }
        }
    }
    
    fclose(f);
}

/* This function reads the output of the SAT solver and
 * it writes it in t.
 */
int cnf_output2sudoku(int** t, int n, char* filename){
    
    FILE *f;
    
    int i, j, k, n2, val;
    double time;
    char sat[10];
    
    n2 = n*n;
    f = fopen(filename,"r");
    
    fscanf(f, "%s%lf", sat, &time);
    
    if (sat[0] == '1'){
        for (i=0; i<n2; i++){
            for (j=0; j<n2; j++){
                for (k=1; k<=n2; k++){
                    fscanf(f, "%d", &val);
                    
                    if (val != 0){
                        t[i][j] = k;
                    }
                }
            }
        }
        
        return 1;
    }
    
    fclose(f);
    
    return 0;
}

void solve_and_read(char* command, int** t, int n){
    
    system(command);
    
    if ( cnf_output2sudoku(t, n, "sudoku.out") ){
        print_sudoku(t, n);
    }
}


int main(int argc, char* argv[]){
    
    FILE *f;
    int** t;
    char s[BUFFERSIZE];
    int n, i, j, index, s_size, n2;
    char* filename = argv[1];
    
    f = fopen(filename, "r");
    
    const clock_t begin_time = clock();
    
    // It reads each instance of sudoku problem
    while(fscanf(f, "%d", &n) != EOF){
        
        system("rm -rf sudoku.cnf result");
        
        n2 = n*n;
        
        memset(s, 0, sizeof s);
        fscanf(f, "%s", s);
        
        // Allocate memmory for the board
        
        t = (int**)malloc(n2*sizeof(int*));
        for (i=0; i<n2; i++){
            t[i] = (int*)malloc(n2*sizeof(int));
        }
        
        index = 0;
        s_size = strlen(s);
        
        // It reads the initial state of each cell in the board
        for (i=0; i<n2; i++){
            for (j=0; j<n2; j++){
                
                int num = 0;
                while( index < s_size && '0' <= s[index] && s[index] <= '9' ){
                    num = 10*num + (s[index++] - '0');
                }
                index++;
                
                t[i][j] = num;
            }
        }
        
        // It transforms the problem of sudoku to a SAT instance.
        
        sudoku2cnf(t, n, "sudoku.cnf");
        
        print_sudoku(t, n);
        
        printf("Solving instance:\n\n");
        
        solve_and_read("./minisat sudoku.cnf sudoku.out > salida_aux; rm salida_aux", t, n);
        
        solve_and_read("../kec_o_sat_s -f sudoku.cnf -o sudoku.out", t, n);
        
        t = (int**)malloc(n2*sizeof(int*));
        for (i=0; i<n2; i++){
            free(t[i]);
        }
        free(t);
    }
    
    printf("%f\n", (float)( clock () - begin_time ) /  CLOCKS_PER_SEC);
    fclose (f);
}