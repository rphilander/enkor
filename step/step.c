#include "step.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* ── cached keywords (initialized lazily) ────────────────────────── */

static Val *KW_EXPR, *KW_ENV, *KW_STACK, *KW_STATUS;
static Val *KW_RUNNING, *KW_DONE, *KW_SUSPENDED;
static Val *KW_EFFECT;
static Val *KW_TYPE, *KW_FN, *KW_PARAMS, *KW_BODY, *KW_CLOSURE_ENV;
static Val *KW_OP, *KW_ARGS, *KW_REST, *KW_FUNC;
static Val *KW_CALL, *KW_IF_BRANCH, *KW_THEN, *KW_ELSE;
static Val *KW_DO_REST, *KW_LET_BINDS, *KW_LET_BODY, *KW_LET_ENV;
static Val *KW_LOOP_BINDS, *KW_LOOP_BODY, *KW_LOOP_ENV;
static Val *KW_EFFECT_NAME, *KW_EFFECT_ARGS;
static Val *KW_ORIG_BINDS;
static Val *SYM_IF, *SYM_FN, *SYM_LET, *SYM_DO, *SYM_QUOTE;
static Val *SYM_LOOP, *SYM_RECUR;

static void init_keywords(void) {
    if (KW_EXPR) return;
    KW_EXPR = val_keyword("expr");
    KW_ENV = val_keyword("env");
    KW_STACK = val_keyword("stack");
    KW_STATUS = val_keyword("status");
    KW_RUNNING = val_keyword("running");
    KW_DONE = val_keyword("done");
    KW_SUSPENDED = val_keyword("suspended");
    KW_EFFECT = val_keyword("effect");
    KW_TYPE = val_keyword("type");
    KW_FN = val_keyword("fn");
    KW_PARAMS = val_keyword("params");
    KW_BODY = val_keyword("body");
    KW_CLOSURE_ENV = val_keyword("env");
    KW_OP = val_keyword("op");
    KW_ARGS = val_keyword("args");
    KW_REST = val_keyword("rest");
    KW_FUNC = val_keyword("func");
    KW_CALL = val_keyword("call");
    KW_IF_BRANCH = val_keyword("if-branch");
    KW_THEN = val_keyword("then");
    KW_ELSE = val_keyword("else");
    KW_DO_REST = val_keyword("do-rest");
    KW_LET_BINDS = val_keyword("let-binds");
    KW_LET_BODY = val_keyword("let-body");
    KW_LET_ENV = val_keyword("let-env");
    KW_LOOP_BINDS = val_keyword("loop-binds");
    KW_LOOP_BODY = val_keyword("loop-body");
    KW_LOOP_ENV = val_keyword("loop-env");
    KW_EFFECT_NAME = val_keyword("effect-name");
    KW_EFFECT_ARGS = val_keyword("effect-args");
    SYM_IF = val_symbol("if");
    SYM_FN = val_symbol("fn");
    SYM_LET = val_symbol("let");
    SYM_DO = val_symbol("do");
    SYM_QUOTE = val_symbol("quote");
    KW_ORIG_BINDS = val_keyword("orig-binds");
    SYM_LOOP = val_symbol("loop");
    SYM_RECUR = val_symbol("recur");
}

/* ── helpers ─────────────────────────────────────────────────────── */

static Val *get(Val *map, Val *key) {
    return val_map_get(map, key);
}

/* Build a state map */
static Val *make_state(Val *expr, Val *env, Val *stack, Val *status) {
    Val *keys[4] = { KW_EXPR, KW_ENV, KW_STACK, KW_STATUS };
    Val *vals[4] = { expr, env, stack, status };
    return val_map(keys, vals, 4);
}

/* Build a state map with an extra :effect key */
static Val *make_suspended_state(Val *expr, Val *env, Val *stack, Val *effect) {
    Val *keys[5] = { KW_EXPR, KW_ENV, KW_STACK, KW_STATUS, KW_EFFECT };
    Val *vals[5] = { expr, env, stack, KW_SUSPENDED, effect };
    return val_map(keys, vals, 5);
}

/* Look up a symbol in the environment (list of maps, inner to outer) */
static Val *env_lookup(Val *env, Val *symbol) {
    size_t n = val_len(env);
    for (size_t i = 0; i < n; i++) {
        Val *scope = val_list_get(env, i);
        Val *v = val_map_get(scope, symbol);
        if (v != NULL) return v;
    }
    return NULL;
}

/* Push a new scope onto the environment (prepend) */
static Val *env_push(Val *env, Val *scope) {
    size_t n = val_len(env);
    Val **items = malloc((n + 1) * sizeof(Val *));
    items[0] = scope;
    for (size_t i = 0; i < n; i++) items[i + 1] = val_list_get(env, i);
    Val *result = val_list(items, n + 1);
    free(items);
    return result;
}

/* Build a list from a subrange of another list */
static Val *list_slice(Val *list, size_t start, size_t end) {
    size_t len = end - start;
    Val **items = malloc(len * sizeof(Val *));
    for (size_t i = 0; i < len; i++) items[i] = val_list_get(list, start + i);
    Val *result = val_list(items, len);
    free(items);
    return result;
}

/* Append a val to a list */
static Val *list_append(Val *list, Val *item) {
    size_t n = val_len(list);
    Val **items = malloc((n + 1) * sizeof(Val *));
    for (size_t i = 0; i < n; i++) items[i] = val_list_get(list, i);
    items[n] = item;
    Val *result = val_list(items, n + 1);
    free(items);
    return result;
}

/* Push a frame onto the stack (prepend) */
static Val *stack_push(Val *stack, Val *frame) {
    return list_append(stack, frame);  /* we'll use end of list as top */
}

/* Pop from stack (remove last) */
static Val *stack_pop(Val *stack, Val **frame_out) {
    size_t n = val_len(stack);
    assert(n > 0);
    *frame_out = val_list_get(stack, n - 1);
    return list_slice(stack, 0, n - 1);
}

/* Check if a val is a specific symbol */
static int is_sym(Val *v, Val *sym) {
    if (val_type(v) != VAL_SYMBOL) return 0;
    return val_cmp(v, sym) == 0;
}

/* Build a frame map with :op and additional key-value pairs */
static Val *make_frame2(Val *op, Val *k1, Val *v1) {
    Val *keys[2] = { KW_OP, k1 };
    Val *vals[2] = { op, v1 };
    return val_map(keys, vals, 2);
}

static Val *make_frame3(Val *op, Val *k1, Val *v1, Val *k2, Val *v2) {
    Val *keys[3] = { KW_OP, k1, k2 };
    Val *vals[3] = { op, v1, v2 };
    return val_map(keys, vals, 3);
}

static Val *make_frame4(Val *op, Val *k1, Val *v1, Val *k2, Val *v2, Val *k3, Val *v3) {
    Val *keys[4] = { KW_OP, k1, k2, k3 };
    Val *vals[4] = { op, v1, v2, v3 };
    return val_map(keys, vals, 4);
}

static Val *make_frame5(Val *op, Val *k1, Val *v1, Val *k2, Val *v2,
                        Val *k3, Val *v3, Val *k4, Val *v4) {
    Val *keys[5] = { KW_OP, k1, k2, k3, k4 };
    Val *vals[5] = { op, v1, v2, v3, v4 };
    return val_map(keys, vals, 5);
}

/* ── step logic ──────────────────────────────────────────────────── */

/* Forward declarations */
static Val *deliver_value(Val *value, Val *env, Val *stack);
static Val *handle_recur(Val *args_list, Val *env, Val *stack);

/* Step when the current :expr needs to be evaluated */
static Val *step_eval(Val *expr, Val *env, Val *stack) {
    ValType t = val_type(expr);

    /* Atoms self-evaluate */
    if (t == VAL_NIL || t == VAL_BOOL || t == VAL_INT || t == VAL_FLOAT ||
        t == VAL_STRING || t == VAL_KEYWORD || t == VAL_BUILTIN ||
        t == VAL_EFFECT || t == VAL_MAP || t == VAL_ERROR) {
        return deliver_value(expr, env, stack);
    }

    /* Symbol: look up in environment */
    if (t == VAL_SYMBOL) {
        Val *v = env_lookup(env, expr);
        if (v == NULL) {
            char buf[256];
            snprintf(buf, sizeof(buf), "undefined symbol: %s", val_as_symbol(expr));
            return val_error(buf);
        }
        return deliver_value(v, env, stack);
    }

    /* List: either a special form or a function call */
    if (t == VAL_LIST) {
        size_t len = val_len(expr);
        if (len == 0) {
            /* empty list evaluates to empty list */
            return deliver_value(expr, env, stack);
        }

        Val *head = val_list_get(expr, 0);

        /* --- special forms --- */

        if (is_sym(head, SYM_QUOTE)) {
            if (len != 2) return val_error("quote: expected 1 argument");
            Val *quoted = val_list_get(expr, 1);
            return deliver_value(quoted, env, stack);
        }

        if (is_sym(head, SYM_IF)) {
            if (len != 4) return val_error("if: expected 3 arguments (condition then else)");
            Val *then_branch = val_list_get(expr, 2);
            Val *else_branch = val_list_get(expr, 3);
            /* push if-branch frame, evaluate condition */
            Val *frame = make_frame3(KW_IF_BRANCH, KW_THEN, then_branch, KW_ELSE, else_branch);
            Val *new_stack = stack_push(stack, frame);
            Val *cond_expr = val_list_get(expr, 1);
            Val *result = make_state(cond_expr, env, new_stack, KW_RUNNING);
            val_release(frame);
            val_release(new_stack);
            return result;
        }

        if (is_sym(head, SYM_DO)) {
            if (len < 2) return val_error("do: expected at least 1 expression");
            if (len == 2) {
                /* single expr in do: just evaluate it */
                Val *body = val_list_get(expr, 1);
                return make_state(body, env, stack, KW_RUNNING);
            }
            /* evaluate first, push rest */
            Val *first = val_list_get(expr, 1);
            Val *rest = list_slice(expr, 2, len);
            Val *frame = make_frame2(KW_DO_REST, KW_REST, rest);
            Val *new_stack = stack_push(stack, frame);
            Val *result = make_state(first, env, new_stack, KW_RUNNING);
            val_release(rest);
            val_release(frame);
            val_release(new_stack);
            return result;
        }

        if (is_sym(head, SYM_LET)) {
            if (len != 3) return val_error("let: expected (let (bindings...) body)");
            Val *bindings = val_list_get(expr, 1);
            Val *body = val_list_get(expr, 2);
            if (val_type(bindings) != VAL_LIST) return val_error("let: bindings must be a list");
            size_t blen = val_len(bindings);
            if (blen % 2 != 0) return val_error("let: odd number of binding forms");
            if (blen == 0) {
                /* no bindings, just evaluate body */
                return make_state(body, env, stack, KW_RUNNING);
            }
            /* evaluate first binding value, push let frame */
            Val *sym = val_list_get(bindings, 0);
            Val *val_expr = val_list_get(bindings, 1);
            if (val_type(sym) != VAL_SYMBOL) return val_error("let: binding name must be a symbol");
            Val *rest_binds = list_slice(bindings, 0, blen); /* full bindings for frame */
            Val *frame = make_frame4(KW_LET_BINDS, KW_REST, rest_binds,
                                     KW_LET_BODY, body, KW_LET_ENV, env);
            Val *new_stack = stack_push(stack, frame);
            Val *result = make_state(val_expr, env, new_stack, KW_RUNNING);
            val_release(rest_binds);
            val_release(frame);
            val_release(new_stack);
            return result;
        }

        if (is_sym(head, SYM_FN)) {
            if (len != 3) return val_error("fn: expected (fn (params...) body)");
            Val *params = val_list_get(expr, 1);
            Val *body = val_list_get(expr, 2);
            if (val_type(params) != VAL_LIST) return val_error("fn: params must be a list");
            /* validate all params are symbols */
            for (size_t i = 0; i < val_len(params); i++) {
                if (val_type(val_list_get(params, i)) != VAL_SYMBOL)
                    return val_error("fn: parameter names must be symbols");
            }
            /* create closure value */
            Val *keys[4] = { KW_TYPE, KW_PARAMS, KW_BODY, KW_CLOSURE_ENV };
            Val *vals[4] = { KW_FN, params, body, env };
            Val *closure = val_map(keys, vals, 4);
            Val *result = deliver_value(closure, env, stack);
            val_release(closure);
            return result;
        }

        if (is_sym(head, SYM_LOOP)) {
            if (len != 3) return val_error("loop: expected (loop (bindings...) body)");
            Val *bindings = val_list_get(expr, 1);
            Val *body = val_list_get(expr, 2);
            if (val_type(bindings) != VAL_LIST) return val_error("loop: bindings must be a list");
            size_t blen = val_len(bindings);
            if (blen % 2 != 0) return val_error("loop: odd number of binding forms");
            if (blen == 0) {
                return make_state(body, env, stack, KW_RUNNING);
            }
            /* evaluate first binding value, push loop frame */
            Val *val_expr = val_list_get(bindings, 1);
            Val *frame = make_frame5(KW_LOOP_BINDS, KW_REST, bindings,
                                     KW_LOOP_BODY, body, KW_LOOP_ENV, env,
                                     KW_ORIG_BINDS, bindings);
            Val *new_stack = stack_push(stack, frame);
            Val *result = make_state(val_expr, env, new_stack, KW_RUNNING);
            val_release(frame);
            val_release(new_stack);
            return result;
        }

        /* --- function call --- */
        /* evaluate head first, then args left-to-right */
        Val *empty = val_list(NULL, 0);
        Val *rest = list_slice(expr, 1, len);
        Val *frame = make_frame3(KW_CALL, KW_ARGS, empty, KW_REST, rest);
        Val *new_stack = stack_push(stack, frame);
        Val *result = make_state(head, env, new_stack, KW_RUNNING);
        val_release(empty);
        val_release(rest);
        val_release(frame);
        val_release(new_stack);
        return result;
    }

    return val_error("step: cannot evaluate this expression type");
}

/* Apply a function (closure or builtin) to evaluated arguments */
static Val *apply_fn(Val *func, Val *args, Val *env, Val *stack) {
    ValType ft = val_type(func);

    if (ft == VAL_BUILTIN) {
        BuiltinFn fn = val_as_builtin(func);
        size_t argc = val_len(args);
        Val **argv = malloc(argc * sizeof(Val *));
        for (size_t i = 0; i < argc; i++) argv[i] = val_list_get(args, i);
        Val *result = fn(argv, argc);
        free(argv);
        if (val_type(result) == VAL_ERROR) return result;
        Val *state = deliver_value(result, env, stack);
        val_release(result);
        return state;
    }

    if (ft == VAL_EFFECT) {
        /* construct effect request and suspend */
        Val *ek[2] = { KW_EFFECT_NAME, KW_EFFECT_ARGS };
        Val *ev[2];
        Val *name_kw = val_keyword(val_as_effect(func));
        ev[0] = name_kw;
        ev[1] = args;
        Val *effect_desc = val_map(ek, ev, 2);
        Val *nil = val_nil();
        Val *result = make_suspended_state(nil, env, stack, effect_desc);
        val_release(name_kw);
        val_release(effect_desc);
        val_release(nil);
        return result;
    }

    if (ft == VAL_MAP) {
        /* check if it's a closure */
        Val *type_val = val_map_get(func, KW_TYPE);
        if (type_val != NULL && val_cmp(type_val, KW_FN) == 0) {
            Val *params = val_map_get(func, KW_PARAMS);
            Val *body = val_map_get(func, KW_BODY);
            Val *closure_env = val_map_get(func, KW_CLOSURE_ENV);

            size_t argc = val_len(args);
            size_t paramc = val_len(params);
            if (argc != paramc) {
                char buf[128];
                snprintf(buf, sizeof(buf), "wrong number of arguments: expected %zu, got %zu",
                         paramc, argc);
                return val_error(buf);
            }

            /* build scope: params → args */
            Val **keys = malloc(paramc * sizeof(Val *));
            Val **vals = malloc(paramc * sizeof(Val *));
            for (size_t i = 0; i < paramc; i++) {
                keys[i] = val_list_get(params, i);
                vals[i] = val_list_get(args, i);
            }
            Val *scope = val_map(keys, vals, paramc);
            free(keys);
            free(vals);

            Val *new_env = env_push(closure_env, scope);
            Val *result = make_state(body, new_env, stack, KW_RUNNING);
            val_release(scope);
            val_release(new_env);
            return result;
        }
    }

    return val_error("not a callable value");
}

/* Deliver a fully evaluated value to the top stack frame */
static Val *deliver_value(Val *value, Val *env, Val *stack) {
    if (val_len(stack) == 0) {
        /* no more frames: we're done */
        return make_state(value, env, stack, KW_DONE);
    }

    Val *frame;
    Val *rest_stack = stack_pop(stack, &frame);
    Val *op = get(frame, KW_OP);

    /* :call frame — evaluating function and arguments */
    if (val_cmp(op, KW_CALL) == 0) {
        Val *args = get(frame, KW_ARGS);
        Val *rest = get(frame, KW_REST);
        Val *func = get(frame, KW_FUNC);

        if (func == NULL) {
            /* we just evaluated the function position */
            if (val_len(rest) == 0) {
                /* no arguments, apply now */
                Val *empty_args = val_list(NULL, 0);
                Val *result = apply_fn(value, empty_args, env, rest_stack);
                val_release(empty_args);
                val_release(rest_stack);
                return result;
            }
            /* save func, evaluate first arg */
            Val *next_arg = val_list_get(rest, 0);
            Val *new_rest = list_slice(rest, 1, val_len(rest));
            Val *new_frame = make_frame4(KW_CALL, KW_FUNC, value,
                                         KW_ARGS, args, KW_REST, new_rest);
            Val *new_stack = stack_push(rest_stack, new_frame);
            Val *result = make_state(next_arg, env, new_stack, KW_RUNNING);
            val_release(new_rest);
            val_release(new_frame);
            val_release(new_stack);
            val_release(rest_stack);
            return result;
        } else {
            /* we evaluated an argument */
            Val *new_args = list_append(args, value);
            if (val_len(rest) == 0) {
                /* all args evaluated, apply */
                Val *result = apply_fn(func, new_args, env, rest_stack);
                val_release(new_args);
                val_release(rest_stack);
                return result;
            }
            /* more args to evaluate */
            Val *next_arg = val_list_get(rest, 0);
            Val *new_rest = list_slice(rest, 1, val_len(rest));
            Val *new_frame = make_frame4(KW_CALL, KW_FUNC, func,
                                         KW_ARGS, new_args, KW_REST, new_rest);
            Val *new_stack = stack_push(rest_stack, new_frame);
            Val *result = make_state(next_arg, env, new_stack, KW_RUNNING);
            val_release(new_args);
            val_release(new_rest);
            val_release(new_frame);
            val_release(new_stack);
            val_release(rest_stack);
            return result;
        }
    }

    /* :if-branch frame */
    if (val_cmp(op, KW_IF_BRANCH) == 0) {
        Val *then_branch = get(frame, KW_THEN);
        Val *else_branch = get(frame, KW_ELSE);
        /* condition is truthy if not nil and not false */
        int truthy = 1;
        if (val_type(value) == VAL_NIL) truthy = 0;
        if (val_type(value) == VAL_BOOL && !val_as_bool(value)) truthy = 0;
        Val *branch = truthy ? then_branch : else_branch;
        Val *result = make_state(branch, env, rest_stack, KW_RUNNING);
        val_release(rest_stack);
        return result;
    }

    /* :do-rest frame */
    if (val_cmp(op, KW_DO_REST) == 0) {
        Val *rest = get(frame, KW_REST);
        size_t rlen = val_len(rest);
        if (rlen == 1) {
            /* last expression */
            Val *last = val_list_get(rest, 0);
            Val *result = make_state(last, env, rest_stack, KW_RUNNING);
            val_release(rest_stack);
            return result;
        }
        /* more to do */
        Val *next = val_list_get(rest, 0);
        Val *new_rest = list_slice(rest, 1, rlen);
        Val *new_frame = make_frame2(KW_DO_REST, KW_REST, new_rest);
        Val *new_stack = stack_push(rest_stack, new_frame);
        Val *result = make_state(next, env, new_stack, KW_RUNNING);
        val_release(new_rest);
        val_release(new_frame);
        val_release(new_stack);
        val_release(rest_stack);
        return result;
    }

    /* :let-binds frame */
    if (val_cmp(op, KW_LET_BINDS) == 0) {
        Val *bindings = get(frame, KW_REST);
        Val *body = get(frame, KW_LET_BODY);
        Val *let_env = get(frame, KW_LET_ENV);

        /* value is the result of evaluating the current binding's value expr */
        Val *sym = val_list_get(bindings, 0);

        /* create a scope with this binding */
        Val *scope = val_map(&sym, &value, 1);
        Val *new_env = env_push(let_env != NULL ? let_env : env, scope);

        size_t blen = val_len(bindings);
        if (blen <= 2) {
            /* no more bindings, evaluate body in new env */
            Val *result = make_state(body, new_env, rest_stack, KW_RUNNING);
            val_release(scope);
            val_release(new_env);
            val_release(rest_stack);
            return result;
        }

        /* more bindings: evaluate next value in updated env */
        Val *next_bindings = list_slice(bindings, 2, blen);
        Val *next_val_expr = val_list_get(next_bindings, 1);
        Val *new_frame = make_frame4(KW_LET_BINDS, KW_REST, next_bindings,
                                     KW_LET_BODY, body, KW_LET_ENV, new_env);
        Val *new_stack = stack_push(rest_stack, new_frame);
        Val *result = make_state(next_val_expr, new_env, new_stack, KW_RUNNING);
        val_release(scope);
        val_release(new_env);
        val_release(next_bindings);
        val_release(new_frame);
        val_release(new_stack);
        val_release(rest_stack);
        return result;
    }

    /* :loop-binds frame */
    if (val_cmp(op, KW_LOOP_BINDS) == 0) {
        Val *bindings = get(frame, KW_REST);
        Val *body = get(frame, KW_LOOP_BODY);
        Val *loop_env = get(frame, KW_LOOP_ENV);
        Val *orig_binds = get(frame, KW_ORIG_BINDS);

        Val *sym = val_list_get(bindings, 0);
        Val *scope = val_map(&sym, &value, 1);
        Val *new_env = env_push(loop_env != NULL ? loop_env : env, scope);

        size_t blen = val_len(bindings);
        if (blen <= 2) {
            /* all bindings evaluated, evaluate body */
            Val *recur_target_op = val_keyword("loop-target");
            Val *target_frame = make_frame4(recur_target_op, KW_REST, orig_binds,
                                            KW_LOOP_BODY, body, KW_LOOP_ENV, loop_env);
            Val *new_stack = stack_push(rest_stack, target_frame);
            Val *result = make_state(body, new_env, new_stack, KW_RUNNING);
            val_release(scope);
            val_release(new_env);
            val_release(recur_target_op);
            val_release(target_frame);
            val_release(new_stack);
            val_release(rest_stack);
            return result;
        }

        /* more bindings */
        Val *next_bindings = list_slice(bindings, 2, blen);
        Val *next_val_expr = val_list_get(next_bindings, 1);
        Val *new_frame = make_frame5(KW_LOOP_BINDS, KW_REST, next_bindings,
                                     KW_LOOP_BODY, body, KW_LOOP_ENV, new_env,
                                     KW_ORIG_BINDS, orig_binds);
        Val *new_stack = stack_push(rest_stack, new_frame);
        Val *result = make_state(next_val_expr, new_env, new_stack, KW_RUNNING);
        val_release(scope);
        val_release(new_env);
        val_release(next_bindings);
        val_release(new_frame);
        val_release(new_stack);
        val_release(rest_stack);
        return result;
    }

    /* :loop-target frame — when loop body completes without recur, pass value through */
    Val *loop_target_op = val_keyword("loop-target");
    if (val_cmp(op, loop_target_op) == 0) {
        val_release(loop_target_op);
        Val *result = deliver_value(value, env, rest_stack);
        val_release(rest_stack);
        return result;
    }
    val_release(loop_target_op);

    /* :recur-args frame */
    Val *recur_op = val_keyword("recur-args");
    if (val_cmp(op, recur_op) == 0) {
        val_release(recur_op);
        Val *args = get(frame, KW_ARGS);
        Val *rest = get(frame, KW_REST);
        Val *new_args = list_append(args, value);

        if (val_len(rest) == 0) {
            /* all recur args evaluated */
            Val *result = handle_recur(new_args, env, rest_stack);
            val_release(new_args);
            val_release(rest_stack);
            return result;
        }
        /* more args to evaluate */
        Val *next = val_list_get(rest, 0);
        Val *new_rest = list_slice(rest, 1, val_len(rest));
        Val *new_frame = make_frame3(val_keyword("recur-args"),
                                     KW_ARGS, new_args, KW_REST, new_rest);
        Val *new_stack = stack_push(rest_stack, new_frame);
        Val *result = make_state(next, env, new_stack, KW_RUNNING);
        val_release(new_args);
        val_release(new_rest);
        val_release(new_frame);
        val_release(new_stack);
        val_release(rest_stack);
        return result;
    }
    val_release(recur_op);

    val_release(rest_stack);
    return val_error("step: unknown stack frame type");
}

/* ── recur handling ──────────────────────────────────────────────── */

static Val *handle_recur(Val *args_list, Val *env __attribute__((unused)), Val *stack) {
    /* Find the loop-target frame on the stack */
    size_t n = val_len(stack);
    if (n == 0) return val_error("recur: not inside a loop");

    Val *frame = val_list_get(stack, n - 1);
    Val *op = get(frame, KW_OP);
    Val *loop_target_op = val_keyword("loop-target");
    int is_target = val_cmp(op, loop_target_op) == 0;
    val_release(loop_target_op);

    if (!is_target) return val_error("recur: not in tail position of loop");

    Val *bindings = get(frame, KW_REST);
    Val *body = get(frame, KW_LOOP_BODY);
    Val *loop_env = get(frame, KW_LOOP_ENV);

    /* verify arg count matches binding count */
    size_t bind_count = val_len(bindings) / 2;
    size_t arg_count = val_len(args_list);
    if (arg_count != bind_count) {
        char buf[128];
        snprintf(buf, sizeof(buf), "recur: expected %zu arguments, got %zu",
                 bind_count, arg_count);
        return val_error(buf);
    }

    /* rebuild the loop with new values */
    /* construct new bindings list: (sym1 val1 sym2 val2 ...) but with
       the values already evaluated, so we wrap them in (quote v) */
    size_t blen = val_len(bindings);
    Val **new_bind_items = malloc(blen * sizeof(Val *));
    for (size_t i = 0; i < bind_count; i++) {
        new_bind_items[i * 2] = val_list_get(bindings, i * 2); /* symbol */
        /* wrap value in (quote v) so it doesn't get re-evaluated */
        Val *qv[2];
        qv[0] = SYM_QUOTE;
        qv[1] = val_list_get(args_list, i);
        new_bind_items[i * 2 + 1] = val_list(qv, 2);
    }
    Val *new_bindings = val_list(new_bind_items, blen);
    for (size_t i = 0; i < bind_count; i++) val_release(new_bind_items[i * 2 + 1]);
    free(new_bind_items);

    /* build (loop new_bindings body) */
    Val *loop_items[3] = { SYM_LOOP, new_bindings, body };
    Val *loop_expr = val_list(loop_items, 3);

    /* pop the loop-target frame */
    Val *rest_stack = list_slice(stack, 0, n - 1);

    Val *result = make_state(loop_expr, loop_env, rest_stack, KW_RUNNING);
    val_release(new_bindings);
    val_release(loop_expr);
    val_release(rest_stack);
    return result;
}

/* ── public API ──────────────────────────────────────────────────── */

Val *step_init(Val *expr, Val *env) {
    init_keywords();
    Val *empty_stack = val_list(NULL, 0);
    Val *state = make_state(expr, env, empty_stack, KW_RUNNING);
    val_release(empty_stack);
    return state;
}

Val *step(Val *state) {
    init_keywords();

    if (val_type(state) != VAL_MAP) return val_error("step: state must be a map");

    Val *status = get(state, KW_STATUS);
    if (status == NULL) return val_error("step: missing :status");
    if (val_cmp(status, KW_DONE) == 0) return val_error("step: already done");
    if (val_cmp(status, KW_SUSPENDED) == 0) return val_error("step: suspended, awaiting effect resolution");
    if (val_cmp(status, KW_RUNNING) != 0) return val_error("step: unknown status");

    Val *expr = get(state, KW_EXPR);
    Val *env = get(state, KW_ENV);
    Val *stack = get(state, KW_STACK);
    if (expr == NULL || env == NULL || stack == NULL)
        return val_error("step: malformed state (missing :expr, :env, or :stack)");

    /* check for recur as a special call form */
    if (val_type(expr) == VAL_LIST && val_len(expr) > 0) {
        Val *head = val_list_get(expr, 0);
        if (is_sym(head, SYM_RECUR)) {
            /* evaluate args first, then handle recur */
            size_t argc = val_len(expr) - 1;
            if (argc == 0) {
                Val *empty = val_list(NULL, 0);
                Val *result = handle_recur(empty, env, stack);
                val_release(empty);
                return result;
            }
            /* need to evaluate args — push a recur-args frame */
            Val *recur_op = val_keyword("recur-args");
            Val *empty = val_list(NULL, 0);
            Val *rest = list_slice(expr, 2, val_len(expr));
            Val *frame = make_frame3(recur_op, KW_ARGS, empty, KW_REST, rest);
            Val *new_stack = stack_push(stack, frame);
            Val *first_arg = val_list_get(expr, 1);
            Val *result = make_state(first_arg, env, new_stack, KW_RUNNING);
            val_release(recur_op);
            val_release(empty);
            val_release(rest);
            val_release(frame);
            val_release(new_stack);
            return result;
        }
    }

    return step_eval(expr, env, stack);
}
