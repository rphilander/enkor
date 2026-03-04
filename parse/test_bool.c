#include "val.h"
#include "parse.h"
#include "../test.h"

void test_bool(void) {
    Val *input = val_string("true false", 10);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 2);

    Val *t = val_list_get(result, 0);
    ASSERT_TYPE(t, VAL_BOOL);
    ASSERT_TRUE(val_as_bool(t) == true);

    Val *f = val_list_get(result, 1);
    ASSERT_TYPE(f, VAL_BOOL);
    ASSERT_TRUE(val_as_bool(f) == false);

    val_release(result);
    val_release(input);
}
