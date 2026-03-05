#include "val.h"
#include "step.h"
#include "../test.h"

static Val *eq_fn(Val **args, size_t argc) {
    if (argc != 2) return val_error("=: expected 2 args");
    return val_bool(val_cmp(args[0], args[1]) == 0);
}

static Val *add_fn(Val **args, size_t argc) {
    if (argc != 2) return val_error("+: expected 2 args");
    return val_int(val_as_int(args[0]) + val_as_int(args[1]));
}

static Val *run(Val *state) {
    Val *kw_s = val_keyword("status"); Val *kw_d = val_keyword("done");
    for (int i = 0; i < 10000; i++) {
        if (val_type(state) == VAL_ERROR) break;
        if (val_cmp(val_map_get(state, kw_s), kw_d) == 0) break;
        Val *n = step(state); val_release(state); state = n;
    }
    val_release(kw_s); val_release(kw_d); return state;
}

void test_loop(void) {
    /*
     * (loop (i 0 sum 0)
     *   (if (= i 5) sum (recur (+ i 1) (+ sum i))))
     * → 0+1+2+3+4 = 10
     */
    Val *sym_eq = val_symbol("=");
    Val *sym_plus = val_symbol("+");
    Val *bi_eq = val_builtin("=", eq_fn);
    Val *bi_plus = val_builtin("+", add_fn);
    Val *keys[2] = { sym_eq, sym_plus };
    Val *vals[2] = { bi_eq, bi_plus };
    Val *scope = val_map(keys, vals, 2);
    Val *env = val_list(&scope, 1);

    /* bindings: (i 0 sum 0) */
    Val *b[4] = { val_symbol("i"), val_int(0), val_symbol("sum"), val_int(0) };
    Val *bindings = val_list(b, 4);

    /* (= i 5) */
    Val *eq_items[3] = { val_symbol("="), val_symbol("i"), val_int(5) };
    Val *cond = val_list(eq_items, 3);

    /* (+ i 1) */
    Val *inc_items[3] = { val_symbol("+"), val_symbol("i"), val_int(1) };
    Val *inc = val_list(inc_items, 3);

    /* (+ sum i) */
    Val *add_items[3] = { val_symbol("+"), val_symbol("sum"), val_symbol("i") };
    Val *add = val_list(add_items, 3);

    /* (recur (+ i 1) (+ sum i)) */
    Val *recur_items[3] = { val_symbol("recur"), inc, add };
    Val *recur_expr = val_list(recur_items, 3);

    /* (if (= i 5) sum (recur ...)) */
    Val *if_items[4] = { val_symbol("if"), cond, val_symbol("sum"), recur_expr };
    Val *body = val_list(if_items, 4);

    /* (loop (i 0 sum 0) body) */
    Val *loop_items[3] = { val_symbol("loop"), bindings, body };
    Val *expr = val_list(loop_items, 3);

    Val *state = run(step_init(expr, env));
    ASSERT_TYPE(state, VAL_MAP);
    Val *k = val_keyword("expr");
    Val *v = val_map_get(state, k);
    ASSERT_TYPE(v, VAL_INT);
    ASSERT_EQ_INT(val_as_int(v), 10);

    val_release(k); val_release(state); val_release(env); val_release(scope);
    val_release(sym_eq); val_release(sym_plus); val_release(bi_eq); val_release(bi_plus);
    val_release(expr); val_release(loop_items[0]); val_release(bindings); val_release(body);
    for (int i = 0; i < 4; i++) val_release(b[i]);
    val_release(cond); for (int i = 0; i < 3; i++) val_release(eq_items[i]);
    val_release(inc); for (int i = 0; i < 3; i++) val_release(inc_items[i]);
    val_release(add); for (int i = 0; i < 3; i++) val_release(add_items[i]);
    val_release(recur_expr); val_release(recur_items[0]);
    val_release(if_items[0]); val_release(if_items[2]);
}
