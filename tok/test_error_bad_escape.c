#include "val.h"
#include "tok.h"
#include "../test.h"

void test_error_bad_escape(void) {
    Val *input = val_string("\"a\\qb\"", 6);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_ERROR);

    const char *msg = val_as_error(result);
    ASSERT_TRUE(strstr(msg, "escape") != NULL);

    val_release(result);
    val_release(input);
}
