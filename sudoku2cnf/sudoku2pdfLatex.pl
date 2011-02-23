#! /usr/bin/perl
# use warnings;
# use strict;

# File: sudoku2pdfLatex.pl
#
# Description:
# 


my ($input_file, $output_file) = @ARGV;
my $case_num = 0;
my $caso = 0;
my @sudoku;
my @current_line;
my ($solver, $time);
my $temp_file = $output_file;
my $i = 0;

open(INPUT_FILE, "<", $input_file ) or die $!;
open(TEMP_FILE, ">", $temp_file) or die $!;

my $string = "\\documentclass{article}
    \\usepackage{sudoku}\\setlength\\sudokusize{10cm}\\begin{document}";
print TEMP_FILE $string;

while( <INPUT_FILE> != 0) {

    $caso++;
    print TEMP_FILE "\\Huge{Caso $caso}";
    
    #my $sudoku_card    = <INPUT_FILE>;
    my @initial_sudoku = split(" ", <INPUT_FILE>);
    my $num_cases = <INPUT_FILE>;
    
    print TEMP_FILE "\\begin{center}{\\huge Sudoku Inicial}\\end{center}";
    print_sudoku(@initial_sudoku);
    print TEMP_FILE "\\section{Resultados}";

    while( $num_cases ) {
        my $sudoku_card = $_;


        chomp($solver = <INPUT_FILE>);
        chomp($time = <INPUT_FILE>);
        print TEMP_FILE "\n{\\subsection*{Soluci\\'on del solver: $solver}\\subsection*{Tiempo: $time ms.}}\n";
        @sudoku = split(" ", <INPUT_FILE>);
        print_sudoku(@sudoku);

#     print TEMP_FILE "\\begin{sudoku}\n";
#     while( @sudoku ) {
#         @current_line = splice( @sudoku, 0, 9); 
#         print "tam sudoku restante: $#sudoku\n";
#         @current_line = join("|", @current_line);
#         unshift  @current_line, "|";
#         push @current_line, "|.\n";
#         print TEMP_FILE  @current_line;
#     }
#     print TEMP_FILE "\\end{sudoku}\n";
        $num_cases--;
        print TEMP_FILE "\\newpage\n";
    }
}
print TEMP_FILE "\\end{document}";

close TEMP_FILE;

system("pdflatex", $temp_file);
#This should be modified!
system("rm -rf *.aux *.log");

sub print_sudoku() {
    @sudoku = @_;

    print TEMP_FILE "\\begin{sudoku}\n";
    while( @sudoku ) {
        @current_line = splice( @sudoku, 0, 9);
        @current_line = join("|", @current_line);
        unshift  @current_line, "|";
        push @current_line, "|.\n";
        print TEMP_FILE  @current_line;
    }
    print TEMP_FILE "\\end{sudoku}\n";
}

# -Primero recibo 1 entero la dim. del sudoku. en una linea el solo
# - Si ese entero =0 se acaba la entrada.
# -En la sig. linea, recibo 81 numeros separados por espacios, el sudoku inicial.
# -Un 0 esa secuencia==> casilla vacia.
# -Despues otro entero en una linea sola el entero m.
# - Despues m soluciones. Cada solucion tiene 3  lineas.
# - Esas tres lineas son: 1ero el nombre del sat solver en una linea.
# La segunda el tiempo en ms en resolver ese soduku.
# -La tercera la solucion 
# -Cabe destacar que si no se resolvio la 3era linea son puros 0.

# 3 dim sudoku
# 1  3 4 5 5 6  sudoku base
# 2 numero de soluciones
# sat1  nombre sat solver.
# 0.001010 tiempo
# 1 2 3 4  5
# sat2
# 3
# 23123 123 12 312
# 0
