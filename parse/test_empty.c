#include "val.h"
#include "parse.h"
#include "../test.h"

void test_empty(void) {
    Val *input = val_string("", 0);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 0);
    val_release(result);
    val_release(input);
}
