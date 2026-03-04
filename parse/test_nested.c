#include "val.h"
#include "parse.h"
#include "../test.h"

void test_nested(void) {
    Val *input = val_string("(if true (+ 1 2) 0)", 19);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);

    Val *outer = val_list_get(result, 0);
    ASSERT_TYPE(outer, VAL_LIST);
    ASSERT_EQ_UINT(val_len(outer), 4);

    ASSERT_EQ_STR(val_as_symbol(val_list_get(outer, 0)), "if");
    ASSERT_TRUE(val_as_bool(val_list_get(outer, 1)) == true);

    Val *inner = val_list_get(outer, 2);
    ASSERT_TYPE(inner, VAL_LIST);
    ASSERT_EQ_UINT(val_len(inner), 3);
    ASSERT_EQ_STR(val_as_symbol(val_list_get(inner, 0)), "+");

    ASSERT_EQ_INT(val_as_int(val_list_get(outer, 3)), 0);

    val_release(result);
    val_release(input);
}
