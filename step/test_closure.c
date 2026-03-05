#include "val.h"
#include "step.h"
#include "../test.h"

static Val *add_fn(Val **args, size_t argc) {
    if (argc != 2) return val_error("+: expected 2 args");
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

void test_closure(void) {
    /* (let (x 10) ((fn (y) (+ x y)) 5)) → 15
       Tests that the closure captures x from the let environment */
    Val *sym_plus = val_symbol("+");
    Val *bi = val_builtin("+", add_fn);
    Val *scope = val_map(&sym_plus, &bi, 1);
    Val *env = val_list(&scope, 1);

    /* (fn (y) (+ x y)) */
    Val *p[1] = { val_symbol("y") };
    Val *params = val_list(p, 1);
    Val *b_items[3] = { val_symbol("+"), val_symbol("x"), val_symbol("y") };
    Val *fn_body = val_list(b_items, 3);
    Val *fn_items[3] = { val_symbol("fn"), params, fn_body };
    Val *fn_expr = val_list(fn_items, 3);

    /* ((fn ...) 5) */
    Val *call_items[2] = { fn_expr, val_int(5) };
    Val *call = val_list(call_items, 2);

    /* (let (x 10) call) */
    Val *binds[2] = { val_symbol("x"), val_int(10) };
    Val *bindings = val_list(binds, 2);
    Val *let_items[3] = { val_symbol("let"), bindings, call };
    Val *expr = val_list(let_items, 3);

    Val *state = run(step_init(expr, env));
    ASSERT_TYPE(state, VAL_MAP);
    Val *k = val_keyword("expr");
    Val *v = val_map_get(state, k);
    ASSERT_EQ_INT(val_as_int(v), 15);

    val_release(k); val_release(state); val_release(env); val_release(scope);
    val_release(bi); val_release(sym_plus);
    val_release(expr); val_release(let_items[0]); val_release(bindings); val_release(call);
    val_release(call_items[1]);
    val_release(fn_expr); val_release(fn_items[0]); val_release(params); val_release(fn_body);
    val_release(p[0]); for (int i = 0; i < 3; i++) val_release(b_items[i]);
    val_release(binds[0]); val_release(binds[1]);
}
