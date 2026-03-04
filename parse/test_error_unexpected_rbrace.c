#include "val.h"
#include "parse.h"
#include "../test.h"

void test_error_unexpected_rbrace(void) {
    Val *input = val_string("}", 1);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_ERROR);
    ASSERT_TRUE(strstr(val_as_error(result), "unexpected '}'") != NULL);
    val_release(result);
    val_release(input);
}
