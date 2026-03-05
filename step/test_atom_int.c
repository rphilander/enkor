#include "val.h"
#include "step.h"
#include "../test.h"

static Val *run(Val *state) {
    Val *kw_status = val_keyword("status");
    Val *kw_done = val_keyword("done");
    for (int i = 0; i < 1000; i++) {
        if (val_type(state) == VAL_ERROR) break;
        Val *status = val_map_get(state, kw_status);
        if (val_cmp(status, kw_done) == 0) break;
        Val *next = step(state);
        val_release(state);
        state = next;
    }
    val_release(kw_status);
    val_release(kw_done);
    return state;
}

static Val *result_expr(Val *state) {
    Val *k = val_keyword("expr");
    Val *v = val_map_get(state, k);
    val_release(k);
    return v;
}

void test_atom_int(void) {
    Val *expr = val_int(42);
    Val *scope = val_map(NULL, NULL, 0);
    Val *env = val_list(&scope, 1);
    Val *state = run(step_init(expr, env));

    ASSERT_TYPE(state, VAL_MAP);
    Val *v = result_expr(state);
    ASSERT_TYPE(v, VAL_INT);
    ASSERT_EQ_INT(val_as_int(v), 42);

    val_release(state);
    val_release(env);
    val_release(scope);
    val_release(expr);
}
