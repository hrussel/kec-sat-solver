/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Definition of the structure used for the SAT solver and signatures of the
 * procedures.
 *
 * The structure used is an array of pointers to clauses, where a clause is
 * an array of literals, and each literal is an integer. Clause watchers are
 * also included, and a stack for performing the non-chronological bactracking.
 *
 * The procedures defined in this file are used to operate and change the SAT
 * structure
 *
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 ******************************************************************************/
#ifndef _KECOSATS_SAT_

#define _KECOSATS_SAT_

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

#define SATISFIED 1
#define UNSATISFIED 0

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

/**
 *
 * This function receives a @e literal and performs Boolean Constrain
 * Propagation (BCP) by calling the appropriate functions that perform Unit
 * Clause Propagation and Pure Literal Elimination.
 *
 * @param literal The variable selected for assignment by the function
 *        @e decide_next_branch.
 * @return Returns a status that indicates how the assignment to variable
 *        @e literal went on. It will return SATISFIED, if after assigning
 *        @e literal a truth value and after Unit Clause propagation and
 *        pure literal elimination has been performed, the formula was
 *        found to be satisfied. It will return DONT_CARE(0), if after assigning
 *        @e literal a truth value and after Unit Clause propagation and
 *        pure literal elimination has been performed, the formula hasn't
 *        still been satisfied and there is still no conflict, so there's the
 *        chance that if we continue to assign some other variables we may find
 *        the formula to be satisfied or conflicted. It will return CONFLICT(2),
 *        if the assignment of @e literal with some value, along with the values
 *        that were previously assigned to some variables is conflictive.
 *        
 */
int deduce(variable literal);

int set_newly_watchers( list* clauses_affected, variable literal );

/**
 * This function receives a stack of unitary clauses whose single variables need
 * to be properly assigned and propagated.
 *    
 * @param unit_clauses A stack of unitary clauses whose single variables need to
 *        be properly assigned and propagated.
 * @return UNIT_CLAUSE(1) If the clause head_clause is unitary.
 *         CONFLICT(2)    If the clause is conflictive with the current model.
 *         DONT_CARE(0)   Neither of the previous two alternatives.
 * @pre unit_clauses != NULL;
 */

int unit_propagation( stack* clauses_not_made_true);

void unassign(variable literal);

/**
 * Decide the next variable for the backtracking to continue,
 * once chosen, push it to the @e backtracking_status stack of the
 * sat_st global variable as a @e decision_level_data structure.
 *
 * The policy for chosing an unset variable is to select the one that has
 * the biggest number of watchers. So as to maximize the number
 * of satisfied clauses in the assingment and detect faster conflicts
 * or satisfability of the formula. Thereby, the time complexity of this
 * procedure is O(num_vars).
 */
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

/**
 * 
 * @param cl A pointer to a clause.
 * @param literal A variable.
 * @return TRUE(1)     if the variable @e literal is the head watcher of @e cl.
 *         FALSE(0)    if the variable @e literal isnt' the head watcher of @e
 *                     cl. 
 */
int is_head_watcher( clause* clause, variable literal );


/**
 * 
 * @param cl A pointer to a clause.
 * @param literal A variable.
 * @return TRUE(1)     if the variable @e literal is the tail watcher of @e cl.
 *         FALSE(0)    if the variable @e literal isnt' the tail watcher of @e 
 *                     cl.
 */
int is_tail_watcher( clause* clause, variable literal );

/**
 * Returns the value in the model for a literal. If this literal hasn't still
 * been assigned it returns UNKNOWN(-1).
 * @param literal A pointer to the variable about which we would like to know
 *        its value.
 * @return TRUE(1)     if the value has been assigned TRUE.
 *         FALSE(0)    if the value has been assigned FALSE.
 *         UNKNOWN(-1) if the value has not been assigned.
 */
int current_literal_value( variable* literal );

/**
 * Given a clause and a freshly recently assigned variable occurring in the
 * aforementioned clause that is being pointed to by the head_watcher, searches
 * for some other valid literal to be the new head_watcher. 
 * This method determines if a clause a unitary clause or a conflictive clause.
 *
 * @param head_clause
 * @return UNIT_CLAUSE(1) If the clause head_clause is unitary.
 *         CONFLICT(2)    If the clause is conflictive with the current model.
 *         DONT_CARE(0)   Neither of the previous two alternatives.
 * @pre head_clause != NULL
 *
 */
int update_watcher( clause* clause );

/**
 * This functions swaps the head_watcher and the tail_watcher of the clause cl.
 *
 * @param cl A clause parameter
 * @pre cl != NULL;
 *
 */
void swap_watchers(clause* cl );

/**
 * The purpose of this procedure is to add the clause cl to the list of
 * clauses where v is watched positively or negatively.
 *
 * @param v The variable that is watched.
 * @param cl A pointer to the clause where v is watched
 *
 */
void add_to_watched_list(variable v, clause* cl);

/**
 * Return true iff the literal v is true with the current assignment.
 *
 * @param v
 * @return UNIT_CLAUSE(1) If the clause head_clause is unitary.
 *         CONFLICT(2)    If the clause is conflictive with the current model.
 *         DONT_CARE(0)   Neither of the previous two alternatives.
 * @pre 1 <= abs(v) <= sat_st.num_vars
 *
 */
int is_satisfied( variable v );

void print_status();

int preprocess();

/**
 * This function tries to solve the sat_instance that is stored
 * in the global variable sat_st.
 *
 * @return   SATISFIABLE if an assignment to the SAT variables is
 *           found that satisfies the formula. Otherwise, UNSATISFIABLE.
 */
int solve_sat();

void print_sol(int status, char filename[]);

SAT_status sat_st;

#endif
