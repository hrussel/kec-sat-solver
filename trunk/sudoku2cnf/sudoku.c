#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFERSIZE 1000000
#define VARIABLE(i,j,k) n2*(n2*i + j) + k
#define FILA(c,i) (n * ( c / n ) + (i / n))
#define COLUMNA(c,i) (n * (c % n) + (i % n))

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

/* Print a sudoku of size n */
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

void sudoku2cnf(int** t, int n, char* filename){
    
    int i, j, k, i1, i2, j1, j2, k1, k2, variables, clausulas, n2;
    int cuadro, celda1, celda2, asig;
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
    clausulas  = n2*n2*(1 + 4*((n2*(n2-1))/2)) + asig;
    
    f = fopen(filename, "w+");
    
    fprintf(f, "p cnf %d %d\n", variables, clausulas);
    
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
    for (cuadro = 0; cuadro < n2; cuadro++){
        for (celda1 = 0; celda1 < n2; celda1++){
            for (celda2 = 0; celda2 < celda1; celda2++){
                for (k = 1; k <= n2; k++){
                    fprintf(f, "-%d -%d 0\n",
                               VARIABLE(FILA(cuadro, celda1),
                                        COLUMNA(cuadro, celda1),
                                        k),
                               VARIABLE(FILA(cuadro, celda2),
                                        COLUMNA(cuadro, celda2),
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

/* Lee la salida del tablero de sudoku t de orden n proveniente
   del solver minisat en el archivo <filename>
 */
int leer_salida(int** t, int n, char* filename){
    
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

int main(int argc, char* argv[]){
    
    FILE *f;
    int** t;
    char s[BUFFERSIZE];
    int n, i, j, index, s_size, n2;
    char* filename = argv[1];
    
    f = fopen(filename, "r");
    
    /* Lectura de cada tablero */
    
    const clock_t begin_time = clock();
    
    while(fscanf(f, "%d", &n) != EOF){
        
        system("rm -rf sudoku.cnf result");
        
        n2 = n*n;
        
        memset(s, 0, sizeof s);
        fscanf(f, "%s", s);
        
        t = (int**)malloc(n2*sizeof(int*));
        for (i=0; i<n2; i++){
            t[i] = (int*)malloc(n2*sizeof(int));
        }
        
        index = 0;
        s_size = strlen(s);
        
        /* Lectura de las casillas del tablero */
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
        
        sudoku2cnf(t, n, "sudoku.cnf");
        
        system("../keck_o_sat_s sudoku.cnf sudoku.out");
        
        system("./minisat sudoku.cnf sudoku.out");
        //system("../kec-sat-solver/lib/sat sudoku.cnf sudoku.out");
        
        print_sudoku(t, n);
        if ( leer_salida(t, n, "sudoku.out") ){
            print_sudoku(t, n);
        }
    }
    
    printf("%f\n", (float)( clock () - begin_time ) /  CLOCKS_PER_SEC);
    fclose (f);
}
