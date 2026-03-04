#include "val.h"
#include "parse.h"
#include "../test.h"

void test_keyword(void) {
    Val *input = val_string(":name", 5);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);
    Val *k = val_list_get(result, 0);
    ASSERT_TYPE(k, VAL_KEYWORD);
    ASSERT_EQ_STR(val_as_keyword(k), "name");
    val_release(result);
    val_release(input);
}
