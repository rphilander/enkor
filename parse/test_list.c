#include "val.h"
#include "parse.h"
#include "../test.h"

void test_list(void) {
    Val *input = val_string("(+ 1 2)", 7);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);

    Val *list = val_list_get(result, 0);
    ASSERT_TYPE(list, VAL_LIST);
    ASSERT_EQ_UINT(val_len(list), 3);
    ASSERT_TYPE(val_list_get(list, 0), VAL_SYMBOL);
    ASSERT_EQ_STR(val_as_symbol(val_list_get(list, 0)), "+");
    ASSERT_EQ_INT(val_as_int(val_list_get(list, 1)), 1);
    ASSERT_EQ_INT(val_as_int(val_list_get(list, 2)), 2);

    val_release(result);
    val_release(input);
}
