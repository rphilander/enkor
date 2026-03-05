#ifndef STEP_H
#define STEP_H

#include "val.h"

/*
 * Step evaluator for enkor.
 *
 * Takes a state Val (a map) and returns a new state Val or a VAL_ERROR.
 *
 * State map keys:
 *   :expr   — the current expression being evaluated
 *   :env    — environment: a VAL_LIST of VAL_MAPs (inner to outer scope)
 *   :stack  — continuation frames: a VAL_LIST
 *   :status — a keyword: :running, :done, or :suspended
 *
 * When :status is :done, the result is in :expr.
 * When :status is :suspended, the effect request is in :effect.
 *
 * Special forms: if, fn, let, do, quote, loop, recur.
 * Builtins (VAL_BUILTIN) and effects (VAL_EFFECT) are resolved from :env.
 */
Val *step(Val *state);

/*
 * Helper: create an initial state for evaluating an expression.
 */
Val *step_init(Val *expr, Val *env);

#endif
