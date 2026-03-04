#include "val.h"
#include "../test.h"

void test_cmp_string(void) {
    Val *a = val_string("abc", 3);
    Val *b = val_string("abd", 3);
    Val *c = val_string("ab", 2);

    ASSERT_CMP_LT(a, b);       /* abc < abd */
    ASSERT_CMP_LT(c, a);       /* ab < abc (prefix) */
    ASSERT_CMP_GT(b, a);

    Val *a2 = val_string("abc", 3);
    ASSERT_CMP_EQ(a, a2);

    val_release(a); val_release(b);
    val_release(c); val_release(a2);
}
