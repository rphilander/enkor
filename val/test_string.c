#include "val.h"
#include "../test.h"

void test_string(void) {
    size_t len;
    Val *v = val_string("hello", 5);
    ASSERT_TYPE(v, VAL_STRING);
    const char *s = val_as_string(v, &len);
    ASSERT_EQ_UINT(len, 5);
    ASSERT_EQ_MEM(s, "hello", 5);
    val_release(v);
}
