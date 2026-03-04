#include "val.h"
#include "tok.h"
#include "../test.h"

void test_booleans(void) {
    Val *input = val_string("true false", 10);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 2);

    Val *k_type = val_keyword("type");
    Val *k_val = val_keyword("value");

    Val *t0 = val_list_get(result, 0);
    Val *ty0 = val_map_get(t0, k_type);
    Val *exp_true = val_keyword("true");
    ASSERT_CMP_EQ(ty0, exp_true);
    Val *v0 = val_map_get(t0, k_val);
    ASSERT_TYPE(v0, VAL_BOOL);
    ASSERT_TRUE(val_as_bool(v0) == true);

    Val *t1 = val_list_get(result, 1);
    Val *ty1 = val_map_get(t1, k_type);
    Val *exp_false = val_keyword("false");
    ASSERT_CMP_EQ(ty1, exp_false);
    Val *v1 = val_map_get(t1, k_val);
    ASSERT_TYPE(v1, VAL_BOOL);
    ASSERT_TRUE(val_as_bool(v1) == false);

    val_release(k_type);
    val_release(k_val);
    val_release(exp_true);
    val_release(exp_false);
    val_release(result);
    val_release(input);
}
