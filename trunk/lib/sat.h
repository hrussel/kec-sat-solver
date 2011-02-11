#ifndef SAT_

    #define SAT_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "list.h"

#define TRUE 1
#define FALSE 0
#define UNKNOWN -1

#define SATISFIABLE 1
#define UNSATISFIABLE 0

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
*  @param head_watcher Pointer to one of the clause's literals, which is
*         being watched.
*  @param tail_watcher Pointer to one of the clause's literals, which is
          being watched.
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
*         variable x_i. This assignment can be only one of the following:
*         TRUE, FALSE, UNKNOWN.
*
*/
typedef struct SAT_status{
    
    int num_vars;
    int num_clauses;
    clause *formula;
    
    list *pos_occurrence_list; //Not necessary
    list *neg_occurrence_list; //Not necessary
    
    list *pos_watched_list;
    list *neg_watched_list;
    
    stack backtracking_status;
    int *model;                     // The current assignment of boolean values
                                    // to literals, that is under study.
} SAT_status;

/**
*  This struct models the applied changes in a certain decision level.
*
*  @param assigned_literal The first literal selected for assignment in a 
*         decision level. This assignment may span other assigned_literals with
*         the aid of Boolean Constrain Propagation (unit_propagation).
*  @param missing_branch true iff we need to test the opposite
*         polarity of the decision variable. Note that the value of this
*         member of the struct should be ignored if the variable @e unassign
*         is set to TRUE.
*  @param propagated_var A list of variables whose assignment was inferred from
*         the assignment of @e assigned_literal.
*         where the variable x_i occurs positively.
*  @param satisfied_clauses A list of the clauses that in the current
*         decision level have already been satisfied.
*/
typedef struct decision_level_data{
    
    variable assigned_literal;
    int missing_branch;             
                                   
    list propagated_var;
    list satisfied_clauses;
    
} decision_level_data;

/**
 * This function destroys an element of type decision_level_data:
 * It destroys its internal lists and also frees its own block of
 * data
 */
void free_decision_level_data(decision_level_data* dld);

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
int deduce(variable literal);

int set_newly_watchers( list* clauses_affected, variable literal );

int unit_propagation( stack* clauses_not_made_true);

void unassign(variable literal);

int decide_next_branch();

int sove_sat();

/**
 * This procedure is used for backtracking purposes.
 * The procedure undoes all the assignments made in the
 * given decision level.
 *
 * @param dec_lev_dat    The structure that describes the operations
 *                       made in the decision level
 */
void undo_assignments(decision_level_data *dec_lev_dat);

int is_head_watcher( clause* clause, variable literal );
int is_tail_watcher( clause* clause, variable literal );

// Returns the value in the model for a literal.
int current_literal_value( variable* literal );

int update_watcher( clause* head_clause );
void swap_watchers(clause* cl );

void add_to_watched_list(variable v, clause* cl);

SAT_status sat_st;

#endif
