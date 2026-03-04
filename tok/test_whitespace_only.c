#include "val.h"
#include "tok.h"
#include "../test.h"

void test_whitespace_only(void) {
    Val *input = val_string("  \t\n  ", 6);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 0);
    val_release(result);
    val_release(input);
}
