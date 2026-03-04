#include "val.h"
#include "tok.h"
#include "../test.h"

void test_negative_float(void) {
    Val *input = val_string("-0.5", 4);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);

    Val *t = val_list_get(result, 0);
    Val *k_val = val_keyword("value");
    Val *v = val_map_get(t, k_val);
    ASSERT_TYPE(v, VAL_FLOAT);
    ASSERT_EQ_FLOAT(val_as_float(v), -0.5);

    val_release(k_val);
    val_release(result);
    val_release(input);
}
