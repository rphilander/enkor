#include "val.h"
#include "tok.h"
#include "../test.h"

void test_error_comma(void) {
    Val *input = val_string("{:a 1, :b 2}", 12);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_ERROR);

    const char *msg = val_as_error(result);
    ASSERT_TRUE(strstr(msg, "comma") != NULL);

    val_release(result);
    val_release(input);
}
