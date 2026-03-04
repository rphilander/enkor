#include "val.h"
#include "../test.h"

static Val *fn_a(Val **args, size_t argc) { (void)args; (void)argc; return val_nil(); }
static Val *fn_b(Val **args, size_t argc) { (void)args; (void)argc; return val_nil(); }

void test_cmp_builtin(void) {
    Val *a = val_builtin("add", fn_a);
    Val *b = val_builtin("add", fn_b);
    Val *c = val_builtin("sub", fn_a);

    /* same name → equal, regardless of function pointer */
    ASSERT_CMP_EQ(a, b);
    /* different names → ordered by name */
    ASSERT_CMP_LT(a, c);  /* "add" < "sub" */
    ASSERT_CMP_GT(c, a);

    val_release(a);
    val_release(b);
    val_release(c);
}
