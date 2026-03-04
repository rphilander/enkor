#include "val.h"
#include "parse.h"
#include "../test.h"

void test_int(void) {
    Val *input = val_string("42 -3", 5);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 2);
    ASSERT_EQ_INT(val_as_int(val_list_get(result, 0)), 42);
    ASSERT_EQ_INT(val_as_int(val_list_get(result, 1)), -3);
    val_release(result);
    val_release(input);
}
