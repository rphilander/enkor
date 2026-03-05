#include "val.h"
#include "step.h"
#include "../test.h"

void test_undefined_symbol(void) {
    Val *sym = val_symbol("x");
    Val *scope = val_map(NULL, NULL, 0);
    Val *env = val_list(&scope, 1);
    Val *state = step_init(sym, env);
    Val *result = step(state);

    ASSERT_TYPE(result, VAL_ERROR);
    ASSERT_TRUE(strstr(val_as_error(result), "undefined symbol") != NULL);

    val_release(result); val_release(state); val_release(env);
    val_release(scope); val_release(sym);
}
