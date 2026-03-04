#include "val.h"
#include "parse.h"
#include "../test.h"

void test_nil(void) {
    Val *input = val_string("nil", 3);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);
    Val *form = val_list_get(result, 0);
    ASSERT_TYPE(form, VAL_NIL);
    val_release(result);
    val_release(input);
}
