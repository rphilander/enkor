#include "val.h"
#include "tok.h"
#include "../test.h"

void test_error_bracket(void) {
    Val *input = val_string("[1 2]", 5);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_ERROR);

    const char *msg = val_as_error(result);
    ASSERT_TRUE(strstr(msg, "brackets") != NULL);

    val_release(result);
    val_release(input);
}
