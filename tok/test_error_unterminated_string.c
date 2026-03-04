#include "val.h"
#include "tok.h"
#include "../test.h"

void test_error_unterminated_string(void) {
    Val *input = val_string("\"hello", 6);
    Val *result = tok(input);
    ASSERT_TYPE(result, VAL_ERROR);

    const char *msg = val_as_error(result);
    ASSERT_TRUE(strstr(msg, "unterminated") != NULL);

    val_release(result);
    val_release(input);
}
