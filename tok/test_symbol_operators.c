#include "val.h"
#include "tok.h"
#include "../test.h"

void test_symbol_operators(void) {
    Val *input = val_string("+ - * /", 7);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 4);

    const char *expected[] = { "+", "-", "*", "/" };
    Val *k_val = val_keyword("value");
    for (int i = 0; i < 4; i++) {
        Val *t = val_list_get(result, i);
        Val *v = val_map_get(t, k_val);
        ASSERT_TYPE(v, VAL_SYMBOL);
        ASSERT_EQ_STR(val_as_symbol(v), expected[i]);
    }

    val_release(k_val);
    val_release(result);
    val_release(input);
}
