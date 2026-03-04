#include "val.h"
#include "../test.h"

static Val *dummy_add(Val **args, size_t argc) {
    (void)args; (void)argc;
    return val_int(0);
}

void test_builtin(void) {
    Val *b = val_builtin("+", dummy_add);
    ASSERT_TYPE(b, VAL_BUILTIN);
    ASSERT_EQ_STR(val_as_builtin_name(b), "+");
    ASSERT_TRUE(val_as_builtin(b) == dummy_add);
    val_release(b);
}
