#include "val.h"
#include "../test.h"

void test_bool(void) {
    Val *t = val_bool(true);
    ASSERT_TYPE(t, VAL_BOOL);
    ASSERT_TRUE(val_as_bool(t) == true);
    val_release(t);

    Val *f = val_bool(false);
    ASSERT_TYPE(f, VAL_BOOL);
    ASSERT_TRUE(val_as_bool(f) == false);
    val_release(f);
}
