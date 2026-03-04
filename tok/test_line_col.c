#include "val.h"
#include "tok.h"
#include "../test.h"

void test_line_col(void) {
    Val *input = val_string("foo\nbar", 7);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 2);

    Val *k_line = val_keyword("line");
    Val *k_col = val_keyword("col");

    Val *t0 = val_list_get(result, 0);
    ASSERT_EQ_INT(val_as_int(val_map_get(t0, k_line)), 1);
    ASSERT_EQ_INT(val_as_int(val_map_get(t0, k_col)), 1);

    Val *t1 = val_list_get(result, 1);
    ASSERT_EQ_INT(val_as_int(val_map_get(t1, k_line)), 2);
    ASSERT_EQ_INT(val_as_int(val_map_get(t1, k_col)), 1);

    val_release(k_line);
    val_release(k_col);
    val_release(result);
    val_release(input);
}
