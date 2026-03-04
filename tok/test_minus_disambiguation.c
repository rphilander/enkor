#include "val.h"
#include "tok.h"
#include "../test.h"

void test_minus_disambiguation(void) {
    /* "- 42" should be symbol '-' followed by int 42 */
    Val *input = val_string("- 42", 4);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 2);

    Val *k_type = val_keyword("type");
    Val *k_val = val_keyword("value");

    Val *t0 = val_list_get(result, 0);
    Val *ty0 = val_map_get(t0, k_type);
    Val *exp_sym = val_keyword("symbol");
    ASSERT_CMP_EQ(ty0, exp_sym);
    Val *v0 = val_map_get(t0, k_val);
    ASSERT_EQ_STR(val_as_symbol(v0), "-");

    Val *t1 = val_list_get(result, 1);
    Val *ty1 = val_map_get(t1, k_type);
    Val *exp_int = val_keyword("int");
    ASSERT_CMP_EQ(ty1, exp_int);
    Val *v1 = val_map_get(t1, k_val);
    ASSERT_EQ_INT(val_as_int(v1), 42);

    val_release(k_type);
    val_release(k_val);
    val_release(exp_sym);
    val_release(exp_int);
    val_release(result);
    val_release(input);
}
