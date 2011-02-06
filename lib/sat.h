#ifndef SAT_

    #define SAT_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

#define TRUE 1
#define FALSE 0
#define UNKNOWN -1

#define DONT_CARE 0
#define UNIT_CLAUSE 1
#define CONFLICT 2

#define BUFFERSIZE 10123

typedef int variable;

typedef list stack;

/**
*
*  This struct models a clause in the formula.
*
*  @param size The number of literals in the clause.
*  @param head_watcher Pointer to one of the clause literals, that is being watched.
*  @param tail_watcher Pointer to one of the clause literals, that is being watched.
*  @param literals An array of the literals occurring in the clause.
*  @param satisfied True iff the clause is already satisfied.
*
*/
typedef struct clause{
    int size;
    variable* head_watcher;
    variable* tail_watcher;
    variable* literals;
    int satisfied;
} clause;

/**
*  This struct models the current state of analysis of the problem. There is a
*  single global variable named sat_st of this type, on which the necessary
*  changes for this analysis will be perfomed.
*
*  @param num_vars The number of distinct literals in the whole formula.
*  @param num_clauses The number of clauses in the formula.
*  @param pos_occurrence_list An array A such that A[i] is the list of clauses 
*         where the variable x_i occurs positively.
*  @param neg_occurrence_list An array A such that A[i] is the list of clauses 
*         where the variable x_i occurs negatively.
*  @param backtracking_status Is a stack that holds the information 
*         corresponding to a decision level. Its elements are of type 
*         decision_level_data.
*  @param model An array A such that A[i] holds the current assignment of 
*         variable x_i. This assignment can be only of the following:
*         TRUE, FALSE, UNKNOWN.
*
*/
typedef struct SAT_status{
    
    int num_vars;
    int num_clauses;
    clause *formula;
    
    list *pos_occurrence_list;
    list *neg_occurrence_list;
    
    stack backtracking_status;
    char *model;                     // The current assignment of boolean values
                                     // to literals, that is under study.
    
} SAT_status;

/**
*  This struct models the applied changes in a certain decision level.
*
*  @param assigned_literal The first literal selected for assignment in a 
*         decision level. This assignment may span other assigned_literals with
*         the aid of Boolean Constrain Propagation (unit_propagation).
*  @param missing_branch true iff we need to test the opposite
*         polarity of the decision variable..
*  @param propagated_var A list of variables whose assignment was inferred from
*         the assignment of @e assigned_literal.
*         where the variable x_i occurs positively.
*  @param satisfied_clauses A list of the clauses that in the current
*         decision level have already been satisfied.
*/
typedef struct decision_level_data{
    
    variable assigned_literal;
    int missing_branch;             // true iff we need to test the opposite
                                    // polarity of the decision variable.
    
    list propagated_var;
    list satisfied_clauses;
    
} decision_level_data;

void set_initial_sat_status(char filename[]);

void allocate_sat_status();

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

void set_newly_satisfied_clauses( list* clauses_made_true );

int set_newly_unsatisfied_clauses( list* clauses_not_made_true,
                                    variable literal );
                                    
int unit_propagation( stack* clauses_not_made_true);

void unassign(variable literal);

int decide_next_branch();

int solver();

int is_head_watcher( clause* clause, variable literal );
int is_tail_watcher( clause* clause, variable literal );

// Returns the value in the model for a literal.
int current_literal_value( variable* literal );

SAT_status sat_st;

#endif
