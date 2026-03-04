#include "val.h"
#include "../test.h"

void test_string_embedded_null(void) {
    size_t len;
    Val *v = val_string("a\0b", 3);
    ASSERT_TYPE(v, VAL_STRING);
    const char *s = val_as_string(v, &len);
    ASSERT_EQ_UINT(len, 3);
    ASSERT_EQ_MEM(s, "a\0b", 3);
    val_release(v);
}
