/*******************************************************************************
 * KECOSATS (Kelwin-Eleazar-Carlos Optimized SAT Solver)
 *
 * Definition of the structures used for the SAT solver. And their basic
 * operations.
 *
 * The structure used is an array of pointers to clauses, where a clause is
 * an array of literals, and each literal is an integer. Clause watchers are
 * also included, and a stack for performing the non-chronological bactracking.
 *
 * @author Colmenares, Carlos (06-39380@usb.ve)
 * @author Fernandez, Kelwin (07-40888@usb.ve)
 * @author Leal, Eleazar (06-39780@usb.ve)
 *
 * @since 1.0
 ******************************************************************************/
#ifndef _KECOSATS_STRUCTS_

#define _KECOSATS_STRUCTS_

#include <stdio.h>
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
*
*  This struct models a node of the implication graph of a sequence of
*  assignments. This nodes correspond to a boolean assignment of a
*  variable @e x_i.
*
*  @param decision_level The decision level of the variable @e x_i.
*  @param conflictive_clause If the value of this field is non-void, then it
*         corresponds to the clause @e cl that makes @x_i to be a unitary
*         variable, thus indicating that @e x_i's assignment was not a decision
*         assignment, but an implication assignment. If this field's value is
*         void  and its decision_level value is distinct from -1, then @e x_i's
*         assignment was a decision assignment.
*
* Note:   If a decision node has -1 value decision_level, then the variable @x_i
*         corresponding to this decision_node is not a node in the current
*         implication graph.
*
*/
typedef struct {
    int decision_level;
    clause* conflictive_clause; // o el indice de la clausula?
} decision_node;

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

typedef struct backtrack_stack{
    int first_free_pos;
    decision_level_data* stack;
} backtrack_stack;

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
*  @param clause_upper_bound An integer such that every learned non-unitary 
*         clause whose size is larger than or equal to it will be eventually
*         deleted.
*  @param clause_available_space Indicates how many clauses be learned, given
*         the current available space in the formula.
*  @param num_original_clauses The number of clauses originally given with the
*         formula.
*  @param impl_graph Keeps the information that makes possible the construction
*         of the implication graph for clause learning.
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
    int clause_upper_bound;
    int clause_available_space;
    int num_original_clauses;
    
    stack unit_learned_clauses;
    
    decision_node *impl_graph;
} SAT_status;

/**
 * This structure defines the different settings that define the behaviour
 * or characteristics of the SAT solver. This settings are setted with
 * the arguments passed to the program when excuted
 *
 *  @param  program_name The name of the program
 *  @param  input_file The path to the input file
 *  @param  output_file The path to the output file. If NULL, the output
 *          is printed through stdout
 *  @param  verbose_mode If set to TRUE, the program will run in verbose
 *          mode and report aditional information when executing
 *  @param  time_out Defines a time limit for the execution of the program;
 *          after time_out seconds have elapsed, the program will abort. If
 *          set to zero, then no time limit is defined
 *  @param  detect_pure_literals If set to true, then run the pure
 *          literals detection algorithm on each iteration of the
 *          backtracking. This option is set to FALSE by default.
 */
typedef struct SAT_global_settings {

    char *program_name;
    char *input_file;
    char *output_file;
    int verbose_mode;
    int time_out;
    int detect_pure_literals;
    int restart_max_unit_clauses;
    
} SAT_global_settings;


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
 * These are the global variablel that all the program will
 * be used in the whole program
 */
SAT_status sat_st;

SAT_global_settings sat_gs;

#endif
