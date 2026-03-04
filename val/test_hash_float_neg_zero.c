#include "val.h"
#include "../test.h"

void test_hash_float_neg_zero(void) {
    Val *a = val_float(0.0);
    Val *b = val_float(-0.0);
    ASSERT_CMP_EQ(a, b);
    ASSERT_EQ_UINT(val_hash(a), val_hash(b));
    val_release(a); val_release(b);
}
