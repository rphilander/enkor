#include "val.h"
#include "../test.h"

void test_cmp_int(void) {
    Val *a = val_int(-10);
    Val *b = val_int(0);
    Val *c = val_int(10);

    ASSERT_CMP_LT(a, b);
    ASSERT_CMP_LT(b, c);
    ASSERT_CMP_GT(c, a);

    Val *b2 = val_int(0);
    ASSERT_CMP_EQ(b, b2);

    val_release(a); val_release(b);
    val_release(c); val_release(b2);
}
