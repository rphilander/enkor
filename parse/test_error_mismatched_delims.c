#include "val.h"
#include "parse.h"
#include "../test.h"

void test_error_mismatched_delims(void) {
    Val *input = val_string("(1 2}", 5);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_ERROR);
    val_release(result);
    val_release(input);
}
