#include "val.h"
#include "step.h"
#include "../test.h"

static Val *run(Val *state) {
    Val *kw_s = val_keyword("status"); Val *kw_d = val_keyword("done");
    for (int i = 0; i < 1000; i++) {
        if (val_type(state) == VAL_ERROR) break;
        if (val_cmp(val_map_get(state, kw_s), kw_d) == 0) break;
        Val *n = step(state); val_release(state); state = n;
    }
    val_release(kw_s); val_release(kw_d); return state;
}

static Val *add_fn(Val **args, size_t argc) {
    if (argc != 2) return val_error("add: expected 2 args");
    return val_int(val_as_int(args[0]) + val_as_int(args[1]));
}

void test_builtin_call(void) {
    /* (+ 1 2) → 3 */
    Val *sym_plus = val_symbol("+");
    Val *builtin = val_builtin("+", add_fn);
    Val *scope = val_map(&sym_plus, &builtin, 1);
    Val *env = val_list(&scope, 1);

    Val *items[3];
    items[0] = val_symbol("+");
    items[1] = val_int(1);
    items[2] = val_int(2);
    Val *expr = val_list(items, 3);

    Val *state = run(step_init(expr, env));
    ASSERT_TYPE(state, VAL_MAP);
    Val *k = val_keyword("expr");
    Val *v = val_map_get(state, k);
    ASSERT_TYPE(v, VAL_INT);
    ASSERT_EQ_INT(val_as_int(v), 3);

    val_release(k); val_release(state); val_release(env);
    val_release(scope); val_release(builtin); val_release(sym_plus);
    val_release(expr);
    for (int i = 0; i < 3; i++) val_release(items[i]);
}
