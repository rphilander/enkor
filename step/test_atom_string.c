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

void test_atom_string(void) {
    Val *expr = val_string("hello", 5);
    Val *scope = val_map(NULL, NULL, 0);
    Val *env = val_list(&scope, 1);
    Val *state = run(step_init(expr, env));
    Val *k = val_keyword("expr");
    Val *v = val_map_get(state, k);
    ASSERT_TYPE(v, VAL_STRING);
    size_t len; ASSERT_EQ_STR(val_as_string(v, &len), "hello");
    val_release(k); val_release(state); val_release(env); val_release(scope); val_release(expr);
}
