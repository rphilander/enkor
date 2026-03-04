#include "val.h"
#include "parse.h"
#include "../test.h"

void test_empty_list(void) {
    Val *input = val_string("()", 2);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);

    Val *list = val_list_get(result, 0);
    ASSERT_TYPE(list, VAL_LIST);
    ASSERT_EQ_UINT(val_len(list), 0);

    val_release(result);
    val_release(input);
}
