#include "val.h"
#include "parse.h"
#include "../test.h"

void test_string(void) {
    Val *input = val_string("\"hello\"", 7);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);
    Val *s = val_list_get(result, 0);
    ASSERT_TYPE(s, VAL_STRING);
    size_t len;
    ASSERT_EQ_STR(val_as_string(s, &len), "hello");
    ASSERT_EQ_UINT(len, 5);
    val_release(result);
    val_release(input);
}
