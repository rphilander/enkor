#include "val.h"
#include "json.h"
#include "../test.h"

void test_int(void) {
    Val *input = val_string("42", 2);
    Val *result = json_to_val(input);
    ASSERT_NOT_ERROR(result);
    ASSERT_TYPE(result, VAL_INT);
    ASSERT_EQ_INT(val_as_int(result), 42);
    val_release(result);
    val_release(input);
}
