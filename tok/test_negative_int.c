#include "val.h"
#include "tok.h"
#include "../test.h"

void test_negative_int(void) {
    Val *input = val_string("-3", 2);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);

    Val *t = val_list_get(result, 0);
    Val *k_val = val_keyword("value");
    Val *v = val_map_get(t, k_val);
    ASSERT_TYPE(v, VAL_INT);
    ASSERT_EQ_INT(val_as_int(v), -3);

    val_release(k_val);
    val_release(result);
    val_release(input);
}
