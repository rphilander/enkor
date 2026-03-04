#include "val.h"
#include "parse.h"
#include "../test.h"

void test_float(void) {
    Val *input = val_string("3.14", 4);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);
    ASSERT_EQ_FLOAT(val_as_float(val_list_get(result, 0)), 3.14);
    val_release(result);
    val_release(input);
}
