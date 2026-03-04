#include "val.h"
#include "parse.h"
#include "../test.h"

void test_multiple_forms(void) {
    Val *input = val_string("(def x 1) (def y 2)", 19);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 2);

    Val *f0 = val_list_get(result, 0);
    ASSERT_TYPE(f0, VAL_LIST);
    ASSERT_EQ_STR(val_as_symbol(val_list_get(f0, 0)), "def");

    Val *f1 = val_list_get(result, 1);
    ASSERT_TYPE(f1, VAL_LIST);
    ASSERT_EQ_STR(val_as_symbol(val_list_get(f1, 0)), "def");

    val_release(result);
    val_release(input);
}
