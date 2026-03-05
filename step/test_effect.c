#include "val.h"
#include "step.h"
#include "../test.h"

static Val *run_until_suspend(Val *state) {
    Val *kw_s = val_keyword("status");
    Val *kw_d = val_keyword("done");
    Val *kw_sus = val_keyword("suspended");
    for (int i = 0; i < 1000; i++) {
        if (val_type(state) == VAL_ERROR) break;
        Val *status = val_map_get(state, kw_s);
        if (val_cmp(status, kw_d) == 0) break;
        if (val_cmp(status, kw_sus) == 0) break;
        Val *n = step(state); val_release(state); state = n;
    }
    val_release(kw_s); val_release(kw_d); val_release(kw_sus);
    return state;
}

void test_effect(void) {
    /* (time-now) should suspend with an effect */
    Val *sym = val_symbol("time-now");
    Val *eff = val_effect("time-now");
    Val *scope = val_map(&sym, &eff, 1);
    Val *env = val_list(&scope, 1);

    Val *call_items[1] = { val_symbol("time-now") };
    Val *expr = val_list(call_items, 1);

    Val *state = run_until_suspend(step_init(expr, env));
    ASSERT_TYPE(state, VAL_MAP);

    Val *kw_status = val_keyword("status");
    Val *status = val_map_get(state, kw_status);
    Val *kw_sus = val_keyword("suspended");
    ASSERT_CMP_EQ(status, kw_sus);

    Val *kw_effect = val_keyword("effect");
    Val *effect = val_map_get(state, kw_effect);
    ASSERT_NOT_NULL(effect);

    Val *kw_name = val_keyword("effect-name");
    Val *name = val_map_get(effect, kw_name);
    Val *exp_name = val_keyword("time-now");
    ASSERT_CMP_EQ(name, exp_name);

    val_release(kw_status); val_release(kw_sus); val_release(kw_effect);
    val_release(kw_name); val_release(exp_name);
    val_release(state); val_release(env); val_release(scope);
    val_release(eff); val_release(sym);
    val_release(expr); val_release(call_items[0]);
}
