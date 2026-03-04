#include "val.h"
#include "../test.h"

void test_cmp_error(void) {
    Val *a = val_error("aaa");
    Val *b = val_error("bbb");
    Val *a2 = val_error("aaa");

    ASSERT_CMP_LT(a, b);
    ASSERT_CMP_GT(b, a);
    ASSERT_CMP_EQ(a, a2);

    /* Error sorts after MAP */
    Val *m = val_map(NULL, NULL, 0);
    ASSERT_CMP_GT(a, m);

    val_release(a); val_release(b);
    val_release(a2); val_release(m);
}
