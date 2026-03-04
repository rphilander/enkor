#include "val.h"
#include "parse.h"
#include "../test.h"

void test_error_unmatched_lparen(void) {
    Val *input = val_string("(+ 1", 4);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_ERROR);
    ASSERT_TRUE(strstr(val_as_error(result), "unmatched '('") != NULL);
    val_release(result);
    val_release(input);
}
