#include "val.h"
#include "tok.h"
#include "../test.h"

void test_comment(void) {
    Val *input = val_string("; this is a comment\n42", 21);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);

    Val *t = val_list_get(result, 0);
    Val *k = val_keyword("type");
    Val *ty = val_map_get(t, k);
    Val *expected = val_keyword("int");
    ASSERT_CMP_EQ(ty, expected);

    val_release(expected);
    val_release(k);
    val_release(result);
    val_release(input);
}
