#include "kecosats_structures.h"
#include "clause_learning.h"
#include <string.h>

#define max(a,b) (a<b?b:a)

/**
 *
 * This function analyses a conflict and returns the decision_level to which
 * the search should backtrack and proceed.
 *
 * If the decision_level equal to 0, it indicates that there's no escape, the
 * conflict is inevitable and thus, the formula is unsatisfiable.
 *
 * @return The decision_level
 *
 */
int analyze_conflict();
