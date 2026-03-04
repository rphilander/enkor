#include "val.h"
#include "../test.h"

void test_error(void) {
    Val *e = val_error("something went wrong");
    ASSERT_TYPE(e, VAL_ERROR);
    ASSERT_EQ_STR(val_as_error(e), "something went wrong");
    val_release(e);
}
