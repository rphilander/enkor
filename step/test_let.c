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

void test_let(void) {
    /* (let (x 10) x) → 10 */
    Val *binds[2]; binds[0] = val_symbol("x"); binds[1] = val_int(10);
    Val *bindings = val_list(binds, 2);
    Val *body = val_symbol("x");
    Val *items[3]; items[0] = val_symbol("let"); items[1] = bindings; items[2] = body;
    Val *expr = val_list(items, 3);

    Val *scope = val_map(NULL, NULL, 0);
    Val *env = val_list(&scope, 1);
    Val *state = run(step_init(expr, env));

    ASSERT_TYPE(state, VAL_MAP);
    Val *k = val_keyword("expr");
    Val *v = val_map_get(state, k);
    ASSERT_TYPE(v, VAL_INT);
    ASSERT_EQ_INT(val_as_int(v), 10);

    val_release(k); val_release(state); val_release(env); val_release(scope);
    val_release(expr); val_release(items[0]); val_release(bindings);
    val_release(body); val_release(binds[0]); val_release(binds[1]);
}
