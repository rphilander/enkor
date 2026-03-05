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

void test_let_multi(void) {
    /* (let (x 10 y 20) (+ x y)) → 30 */
    Val *sym_plus = val_symbol("+");
    Val *bi = val_builtin("+", add_fn);
    Val *scope = val_map(&sym_plus, &bi, 1);
    Val *env = val_list(&scope, 1);

    Val *binds[4] = { val_symbol("x"), val_int(10), val_symbol("y"), val_int(20) };
    Val *bindings = val_list(binds, 4);
    Val *body_items[3] = { val_symbol("+"), val_symbol("x"), val_symbol("y") };
    Val *body = val_list(body_items, 3);
    Val *items[3] = { val_symbol("let"), bindings, body };
    Val *expr = val_list(items, 3);

    Val *state = run(step_init(expr, env));
    ASSERT_TYPE(state, VAL_MAP);
    Val *k = val_keyword("expr");
    Val *v = val_map_get(state, k);
    ASSERT_EQ_INT(val_as_int(v), 30);

    val_release(k); val_release(state); val_release(env); val_release(scope);
    val_release(bi); val_release(sym_plus);
    val_release(expr); val_release(items[0]); val_release(bindings); val_release(body);
    for (int i = 0; i < 4; i++) val_release(binds[i]);
    for (int i = 0; i < 3; i++) val_release(body_items[i]);
}
