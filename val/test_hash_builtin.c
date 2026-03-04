#include "val.h"
#include "../test.h"

static Val *fn_a(Val **args, size_t argc) { (void)args; (void)argc; return val_nil(); }
static Val *fn_b(Val **args, size_t argc) { (void)args; (void)argc; return val_nil(); }

void test_hash_builtin(void) {
    Val *a = val_builtin("add", fn_a);
    Val *b = val_builtin("add", fn_b);
    Val *c = val_builtin("sub", fn_a);

    /* same name → same hash */
    ASSERT_TRUE(val_hash(a) == val_hash(b));
    /* different name → different hash (not guaranteed but extremely likely) */
    ASSERT_TRUE(val_hash(a) != val_hash(c));

    val_release(a);
    val_release(b);
    val_release(c);
}
