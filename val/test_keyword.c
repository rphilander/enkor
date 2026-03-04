#include "val.h"
#include "../test.h"

void test_keyword(void) {
    Val *v = val_keyword("bar");
    ASSERT_TYPE(v, VAL_KEYWORD);
    ASSERT_EQ_STR(val_as_keyword(v), "bar");
    val_release(v);
}
