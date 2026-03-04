#include "val.h"
#include "tok.h"
#include "../test.h"

void test_symbol(void) {
    Val *input = val_string("foo", 3);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);

    Val *t = val_list_get(result, 0);
    Val *k_type = val_keyword("type");
    Val *ty = val_map_get(t, k_type);
    Val *exp_type = val_keyword("symbol");
    ASSERT_CMP_EQ(ty, exp_type);

    Val *k_val = val_keyword("value");
    Val *v = val_map_get(t, k_val);
    ASSERT_TYPE(v, VAL_SYMBOL);
    ASSERT_EQ_STR(val_as_symbol(v), "foo");

    val_release(k_type);
    val_release(exp_type);
    val_release(k_val);
    val_release(result);
    val_release(input);
}
