#include "val.h"
#include "../test.h"

void test_cmp_effect(void) {
    Val *a = val_effect("def");
    Val *b = val_effect("def");
    Val *c = val_effect("time-now");

    ASSERT_CMP_EQ(a, b);
    ASSERT_CMP_LT(a, c);  /* "def" < "time-now" */
    ASSERT_CMP_GT(c, a);

    val_release(a);
    val_release(b);
    val_release(c);
}
