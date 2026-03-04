#include "val.h"
#include "../test.h"

void test_int(void) {
    Val *v = val_int(42);
    ASSERT_TYPE(v, VAL_INT);
    ASSERT_EQ_INT(val_as_int(v), 42);
    val_release(v);

    Val *neg = val_int(-100);
    ASSERT_EQ_INT(val_as_int(neg), -100);
    val_release(neg);

    Val *zero = val_int(0);
    ASSERT_EQ_INT(val_as_int(zero), 0);
    val_release(zero);
}
