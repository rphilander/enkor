#include "val.h"
#include "parse.h"
#include "../test.h"

void test_error_tok_forwarded(void) {
    /* brackets are rejected by tok, error should be forwarded */
    Val *input = val_string("[1 2]", 5);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_ERROR);
    ASSERT_TRUE(strstr(val_as_error(result), "brackets") != NULL);
    val_release(result);
    val_release(input);
}
