#include "val.h"
#include "../test.h"

void test_cmp_bool(void) {
    Val *f = val_bool(false);
    Val *t = val_bool(true);

    ASSERT_CMP_LT(f, t);
    ASSERT_CMP_GT(t, f);
    ASSERT_CMP_EQ(f, f);
    ASSERT_CMP_EQ(t, t);

    Val *f2 = val_bool(false);
    ASSERT_CMP_EQ(f, f2);

    val_release(f); val_release(t); val_release(f2);
}
