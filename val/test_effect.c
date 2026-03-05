#include "val.h"
#include "../test.h"

void test_effect(void) {
    Val *e = val_effect("time-now");
    ASSERT_TYPE(e, VAL_EFFECT);
    ASSERT_EQ_STR(val_as_effect(e), "time-now");
    val_release(e);
}
