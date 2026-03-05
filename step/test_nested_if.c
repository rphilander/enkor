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

void test_nested_if(void) {
    /* (if true (if false 1 2) 3) → 2 */
    Val *inner[4] = { val_symbol("if"), val_bool(false), val_int(1), val_int(2) };
    Val *inner_if = val_list(inner, 4);
    Val *outer[4] = { val_symbol("if"), val_bool(true), inner_if, val_int(3) };
    Val *expr = val_list(outer, 4);

    Val *scope = val_map(NULL, NULL, 0);
    Val *env = val_list(&scope, 1);
    Val *state = run(step_init(expr, env));

    Val *k = val_keyword("expr");
    Val *v = val_map_get(state, k);
    ASSERT_EQ_INT(val_as_int(v), 2);

    val_release(k); val_release(state); val_release(env); val_release(scope);
    val_release(expr); for (int i = 0; i < 4; i++) val_release(outer[i]);
    for (int i = 0; i < 4; i++) val_release(inner[i]);
}
