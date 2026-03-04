#include "val.h"
#include "../test.h"

void test_float(void) {
    Val *v = val_float(3.14);
    ASSERT_TYPE(v, VAL_FLOAT);
    ASSERT_EQ_FLOAT(val_as_float(v), 3.14);
    val_release(v);

    Val *neg = val_float(-2.5);
    ASSERT_EQ_FLOAT(val_as_float(neg), -2.5);
    val_release(neg);

    Val *zero = val_float(0.0);
    ASSERT_EQ_FLOAT(val_as_float(zero), 0.0);
    val_release(zero);
}
