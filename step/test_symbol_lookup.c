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

void test_symbol_lookup(void) {
    Val *sym = val_symbol("x");
    Val *forty_two = val_int(42);
    Val *scope = val_map(&sym, &forty_two, 1);
    Val *env = val_list(&scope, 1);
    Val *state = run(step_init(sym, env));

    Val *k = val_keyword("expr");
    Val *v = val_map_get(state, k);
    ASSERT_TYPE(v, VAL_INT);
    ASSERT_EQ_INT(val_as_int(v), 42);

    val_release(k); val_release(state); val_release(env);
    val_release(scope); val_release(forty_two); val_release(sym);
}
