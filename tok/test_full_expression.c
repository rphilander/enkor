#include "val.h"
#include "tok.h"
#include "../test.h"

void test_full_expression(void) {
    Val *input = val_string("(+ 1 2.0)", 9);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 5);

    Val *k_type = val_keyword("type");

    const char *expected[] = { "lparen", "symbol", "int", "float", "rparen" };
    for (int i = 0; i < 5; i++) {
        Val *t = val_list_get(result, i);
        Val *ty = val_map_get(t, k_type);
        Val *exp = val_keyword(expected[i]);
        ASSERT_CMP_EQ(ty, exp);
        val_release(exp);
    }

    val_release(k_type);
    val_release(result);
    val_release(input);
}
