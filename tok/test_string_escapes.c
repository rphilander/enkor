#include "val.h"
#include "tok.h"
#include "../test.h"

void test_string_escapes(void) {
    /* "a\nb\\c\"d\te" */
    Val *input = val_string("\"a\\nb\\\\c\\\"d\\te\"", 15);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);

    Val *t = val_list_get(result, 0);
    Val *k_val = val_keyword("value");
    Val *v = val_map_get(t, k_val);
    size_t len;
    const char *s = val_as_string(v, &len);
    ASSERT_EQ_UINT(len, 9);
    ASSERT_EQ_MEM(s, "a\nb\\c\"d\te", 9);

    val_release(k_val);
    val_release(result);
    val_release(input);
}
