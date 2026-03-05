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

void test_if_false(void) {
    /* (if false 1 2) → 2 */
    Val *items[4];
    items[0] = val_symbol("if");
    items[1] = val_bool(false);
    items[2] = val_int(1);
    items[3] = val_int(2);
    Val *expr = val_list(items, 4);

    Val *scope = val_map(NULL, NULL, 0);
    Val *env = val_list(&scope, 1);
    Val *state = run(step_init(expr, env));

    Val *k = val_keyword("expr");
    Val *v = val_map_get(state, k);
    ASSERT_TYPE(v, VAL_INT);
    ASSERT_EQ_INT(val_as_int(v), 2);

    val_release(k); val_release(state); val_release(env); val_release(scope);
    val_release(expr); for (int i = 0; i < 4; i++) val_release(items[i]);
}
