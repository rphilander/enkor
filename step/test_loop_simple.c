#include "val.h"
#include "step.h"
#include "../test.h"

static Val *run(Val *state) {
    Val *kw_s = val_keyword("status"); Val *kw_d = val_keyword("done");
    for (int i = 0; i < 1000; i++) {
        if (val_type(state) == VAL_ERROR) { break; }
        if (val_cmp(val_map_get(state, kw_s), kw_d) == 0) break;
        Val *n = step(state); val_release(state); state = n;
    }
    val_release(kw_s); val_release(kw_d); return state;
}

void test_loop_simple(void) {
    /* (loop (x 42) x) → 42 — simplest loop, no recur */
    Val *b[2] = { val_symbol("x"), val_int(42) };
    Val *bindings = val_list(b, 2);
    Val *body = val_symbol("x");
    Val *items[3] = { val_symbol("loop"), bindings, body };
    Val *expr = val_list(items, 3);

    Val *scope = val_map(NULL, NULL, 0);
    Val *env = val_list(&scope, 1);
    Val *state = run(step_init(expr, env));

    if (val_type(state) == VAL_ERROR) {
        fprintf(stderr, "ERROR: %s\n", val_as_error(state));
    }
    ASSERT_TYPE(state, VAL_MAP);
    Val *k = val_keyword("expr");
    Val *v = val_map_get(state, k);
    ASSERT_TYPE(v, VAL_INT);
    ASSERT_EQ_INT(val_as_int(v), 42);

    val_release(k); val_release(state); val_release(env); val_release(scope);
    val_release(expr); val_release(items[0]); val_release(bindings);
    val_release(body); val_release(b[0]); val_release(b[1]);
}
