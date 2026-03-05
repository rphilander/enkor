#include "val.h"
#include "step.h"
#include "../test.h"

static Val *add_fn(Val **args, size_t argc) {
    if (argc != 2) return val_error("add: expected 2 args");
    return val_int(val_as_int(args[0]) + val_as_int(args[1]));
}

static Val *run(Val *state) {
    Val *kw_s = val_keyword("status"); Val *kw_d = val_keyword("done");
    for (int i = 0; i < 1000; i++) {
        if (val_type(state) == VAL_ERROR) break;
        if (val_cmp(val_map_get(state, kw_s), kw_d) == 0) break;
        Val *n = step(state); val_release(state); state = n;
    }
    val_release(kw_s); val_release(kw_d); return state;
}

void test_fn_call(void) {
    /* ((fn (x y) (+ x y)) 3 4) → 7 */
    Val *sym_plus = val_symbol("+");
    Val *builtin = val_builtin("+", add_fn);
    Val *scope = val_map(&sym_plus, &builtin, 1);
    Val *env = val_list(&scope, 1);

    /* build (fn (x y) (+ x y)) */
    Val *params_items[2] = { val_symbol("x"), val_symbol("y") };
    Val *params = val_list(params_items, 2);
    Val *body_items[3] = { val_symbol("+"), val_symbol("x"), val_symbol("y") };
    Val *body = val_list(body_items, 3);
    Val *fn_items[3] = { val_symbol("fn"), params, body };
    Val *fn_expr = val_list(fn_items, 3);

    /* build ((fn ...) 3 4) */
    Val *call_items[3] = { fn_expr, val_int(3), val_int(4) };
    Val *expr = val_list(call_items, 3);

    Val *state = run(step_init(expr, env));
    ASSERT_TYPE(state, VAL_MAP);
    Val *k = val_keyword("expr");
    Val *v = val_map_get(state, k);
    ASSERT_TYPE(v, VAL_INT);
    ASSERT_EQ_INT(val_as_int(v), 7);

    val_release(k); val_release(state); val_release(env); val_release(scope);
    val_release(builtin); val_release(sym_plus);
    val_release(expr); val_release(call_items[1]); val_release(call_items[2]);
    val_release(fn_expr); val_release(fn_items[0]); val_release(params); val_release(body);
    val_release(params_items[0]); val_release(params_items[1]);
    val_release(body_items[0]); val_release(body_items[1]); val_release(body_items[2]);
}
