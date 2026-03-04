#include "val.h"
#include "parse.h"
#include "../test.h"

void test_symbol(void) {
    Val *input = val_string("foo", 3);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);
    Val *s = val_list_get(result, 0);
    ASSERT_TYPE(s, VAL_SYMBOL);
    ASSERT_EQ_STR(val_as_symbol(s), "foo");
    val_release(result);
    val_release(input);
}
