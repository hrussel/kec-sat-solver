#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#define BUFFERSIZE 1000000
#define VARIABLE(i,j,k) n2*(n2*i + j) + k
#define ROW(c,i) (n * ( c / n ) + (i / n))
#define COLUMN(c,i) (n * (c % n) + (i % n))

char* input_filename;
char* output_filename;
char* output_pdf_filename;
int remove_files;
char command1[10000];
char command2[10000];
char command3[10000];
char command4[10000];

struct timeval t_p; 

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

void print_sudoku_pdf(int** t, int n, FILE* fd){
    int i, j;
    int n2 = n*n;
    
    for (i=0; i<n2; i++){
        for (j=0; j<n2; j++){
            fprintf(fd, "%d ", t[i][j]);
        }
    }
    fprintf(fd, "\n");
}

/* It generates a CNF formula corresponding
 * to the sudoku board of size n contained
 * in t.
 * 
 * The instance will be written in the file
 * "filename"
 */
void sudoku2cnf(int** t, int n, char* filename){
    
    int i, j, k, k1, k2, variables, clauses, n2;
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
int cnf_output2sudoku(int** t, int n, char* filename, double* time){
    
    FILE *f;
    
    int i, j, k, n2, val;
    char sat[10];
    
    n2 = n*n;
    f = fopen(filename,"r");
    
    for (i=0; i<n2; i++){
        for (j=0; j<n2; j++){
            t[i][j] = 0;
        }
    }
    
    if ( f == NULL ){
        return 0;
    }
    
    fscanf(f, "%s%lf", sat, time);
    
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

void print_help(){
    
    printf("usage: sudoku -f <input_file_sudoku> -o <output_dimacs_cnf_formula> [OPTIONS]\n");
    printf("\n");
    printf("    OPTIONS:\n");
    printf("\n");
    printf("    -h                Print this message\n");
    printf("\n");
    printf("    -e <output_pdf_file>  Write results in a pdf file <output_file>\n");
    printf("                      if it's not provided, the formula will not be\n");
    printf("                      verified\n");
    printf("    -t <time>         Abort the execution of the program after\n");
    printf("                      'time_out'seconds have elapsed\n");
    
    exit(1);
}

void parse_args(int argc, char* argv[]){
    
    int i=1;
    input_filename = NULL;
    output_filename = NULL;
    output_pdf_filename = NULL;
    remove_files = 1;
    
    int time_limit = -1;
    
    while ( i < argc ){
        if ( strcmp(argv[i], "-f") == 0 ){
            if ( i == argc ){
                print_help();
            }
            
            input_filename = argv[++i];
            
        } else if ( strcmp(argv[i], "-o") == 0){
            if ( i == argc ){
                print_help();
            }
            
            output_filename = argv[++i];
            
        } else if ( strcmp(argv[i], "-e") == 0){
            if ( i == argc ){
                print_help();
            }
            
            output_pdf_filename = argv[++i];
            
            sprintf(command1,
                    "./kec_o_sat_s -f %s -o sudoku.out ",
                    output_filename);
            sprintf(command2,
                    "./sudoku2cnf/zchaff/zchaff %s ",
                    output_filename);
        } else if ( strcmp(argv[i], "-t") == 0){
            
            if ( i == argc ){
                print_help();
            }
            
            time_limit = atoi(argv[++i]);
            
        } else {
            print_help();
        }
        
        i++;
    }
    
    if ( input_filename == NULL || output_filename == NULL ){
        print_help();
    }
    
    if ( time_limit != -1  && output_pdf_filename != NULL){
        char tl[10];
        memset(tl, 0, sizeof tl);
        sprintf(tl, "-t %d", time_limit);
        strcat(command1, tl);
        
        memset(tl, 0, sizeof tl);
        sprintf(tl, "%d", time_limit);
        strcat(command2, tl);
    }
    
    strcat(command2, " > sudoku.out2");
    strcpy(command4, command1);
    strcat(command4, " -hr 0");
    sprintf(command3, "%s -hr 1", command1);
    
    strcat(command1, " > salida_aux; rm -rf salida_aux");
    strcat(command4, " > salida_aux; rm -rf salida_aux");
    strcat(command3, " > salida_aux; rm -rf salida_aux");
}

void solve_and_read(char* command, int** t, int n, char* solver, FILE* in_pdf){
    
    double t_inicial, t_final;
    
    if (!gettimeofday(&t_p,NULL))
       t_inicial = (double) t_p.tv_sec + ((double) t_p.tv_usec)/1000000.0;
    else {
        printf("Error: Bad time request.\n");
        exit(2);
    }
   
    system(command);
        
    if (!gettimeofday(&t_p,NULL))
        t_final = (double) t_p.tv_sec + ((double) t_p.tv_usec)/1000000.0;
    else {
        printf("Error: Bad time request.\n");
        exit(2);
    }
    
    double time = 0;
    if (cnf_output2sudoku(t, n, "sudoku.out", &time)){
        
        time = t_final - t_inicial;
        fprintf(in_pdf, "%s\n%1.4lf\n", solver, time);
        
        print_sudoku_pdf(t, n, in_pdf);
    } else {
        
        time = -1;
        fprintf(in_pdf, "%s\n-1\n", solver);
        
        int n2 = n*n;
        n2 *= n2;
        while(n2--){
            fprintf(in_pdf, "0 ");
        }
        fprintf(in_pdf, "\n");
    }
    
    printf("%1.4lf ", time);
}

int main(int argc, char* argv[]){
    
    FILE *f, *in_pdf;
    int** t;
    char s[BUFFERSIZE];
    int n, i, j, index, s_size, n2;
    
    parse_args(argc, argv);
    
    f = fopen(input_filename, "r");
    
    if ( f == NULL ){
        printf("Error: can't open file %s.\n", input_filename);
        exit(1);
    }
    
    if ( output_pdf_filename != NULL ){
        in_pdf = fopen("auxiliar_pdf", "w");
        
        if ( in_pdf == NULL ){
            printf("Error: can't open file %s.\n", output_pdf_filename);
            exit(1);
        }
        
        printf("Report: zchaff , kecosats-Greedy, Berkmin, Kecosats (seconds)\n");
    }
    
    // It reads each instance of sudoku problem
    
    int T = 0;
    while(fscanf(f, "%d", &n) != EOF){
        
        printf("Sudoku #%d\n", ++T);
        
        system("rm -rf sudoku.cnf");
        
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
        
        sudoku2cnf(t, n, output_filename);
        
        // Call the SAT solvers with the cnf formula.
        
        if ( output_pdf_filename != NULL){
            
            fprintf(in_pdf, "%d\n", n);
            print_sudoku_pdf(t, n, in_pdf);
            
            fprintf(in_pdf, "4\n", n);
            
            // Solving with ZCHAFF
            char commandz[10000];
            memset(commandz, 0, sizeof(command3));
            sprintf(commandz,
                    "%s ; ./sudoku2cnf/parse_zchaff_output %d sudoku.out2 sudoku.out",
                    command2, (n*n)*(n*n)*(n*n));
            
            solve_and_read(commandz, t, n, "zchaff", in_pdf);
            system("rm -rf sudoku.out sudoku.out2");
            
            printf("\n");
            //print_sudoku(t,n);
            // Solving with KEC_O_SAT_S
            
            //printf("\n");
            solve_and_read(command4, t, n, "kecosats greedy", in_pdf);
            
            printf("\n");
            //print_sudoku(t,n);
            
            system("rm -rf sudoku.out");
            
            solve_and_read(command3, t, n, "kecosats berkmin", in_pdf);
            
            printf("\n");
            //print_sudoku(t,n);
            
            system("rm -rf sudoku.out");
            
            solve_and_read(command1, t, n, "kecosats kecosats", in_pdf);
            
            printf("\n");
            //print_sudoku(t,n);
            
            system("rm -rf sudoku.out");
            
        }
        
        // Free memory allocated to the sudoku board.
        
        for (i=0; i<n2; i++){
            free(t[i]);
        }
        free(t);
        
        //char aux;
        //scanf("%c",&aux);
    }
    
    // Generate the pdf
    
    if ( output_pdf_filename != NULL ){
        fprintf(in_pdf, "0\n");
        fclose(in_pdf);
        
        char command[1000];
        memset(command, 0, sizeof command);
        sprintf(command, "perl ./sudoku2cnf/sudoku2pdfLatex.pl auxiliar_pdf sudoku.tex");
        system(command);
        
        memset(command, 0, sizeof command);
        sprintf(command, "cp sudoku.pdf %s", output_pdf_filename);
        system(command);
        
        system("rm -rf auxiliar_pdf sudoku.out sudoku.out2");
    }
    
    fclose (f);
}
