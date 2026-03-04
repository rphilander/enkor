#include "val.h"
#include "../test.h"

void test_nil(void) {
    Val *v = val_nil();
    ASSERT_TYPE(v, VAL_NIL);
    val_release(v);
}
