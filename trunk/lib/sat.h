#ifndef SAT_

    #define SAT_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#define TRUE 1
#define FALSE 0
#define UNKNOWN -1

#define CONFLICT 1

#define BUFFERSIZE 10123

typedef int variable;

typedef list stack;

typedef struct clause{
    int size;
    variable* head_watcher;
    variable* tail_watcher;
    variable* literals;
    int satisfied;
} clause;

typedef struct SAT_status{
    
    int num_vars, num_clauses;
    clause *formula;
    
    list *pos_occurrence_list;
    list *neg_occurrence_list;
    
    stack backtracking_status;
    char *model;                     // The current assignment of boolean values
                                     // to literals, that is under study.
    int status;
    
} SAT_status;

typedef struct decision_level_data{
    
    variable assigned_literal;
    int missing_branch;             // true iff we need to test the opposite
                                    // polarity of the decision variable.
    
    list propagated_var;
    list satisfied_clauses;
    
} decision_level_data;

void set_initial_sat_status(char filename[]);

void set_clause(clause* cl, int clause_length, int literals[]);

void print_formula();

/* Asigna la variable con su polaridad correspondiente en la formula.

   Status:
   Si la formula es satisfecha retornar TRUE
   Si hay contradiccion retornar FALSE
   En caso contrario retornar UNKNOWN
   
   Asignar nuevos watched literals en las clausulas en las que ella sea un
   watched literal.
 */
int assign(variable literal);

void unassign(variable literal);

int decide_next_branch();

int solver();

SAT_status sat_st;

#endif
