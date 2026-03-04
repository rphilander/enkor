#include "val.h"
#include "../test.h"

void test_symbol(void) {
    Val *v = val_symbol("foo");
    ASSERT_TYPE(v, VAL_SYMBOL);
    ASSERT_EQ_STR(val_as_symbol(v), "foo");
    val_release(v);
}
