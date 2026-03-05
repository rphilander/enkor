#include "val.h"
#include "json.h"
#include "../test.h"

void test_string(void) {
    Val *input = val_string("\"hello\"", 7);
    Val *result = json_to_val(input);
    ASSERT_NOT_ERROR(result);
    ASSERT_TYPE(result, VAL_STRING);
    size_t len;
    ASSERT_EQ_STR(val_as_string(result, &len), "hello");
    val_release(result);
    val_release(input);
}
