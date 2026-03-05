#include "val.h"
#include "json.h"
#include "../test.h"

void test_float(void) {
    Val *input = val_string("3.14", 4);
    Val *result = json_to_val(input);
    ASSERT_NOT_ERROR(result);
    ASSERT_TYPE(result, VAL_FLOAT);
    ASSERT_EQ_FLOAT(val_as_float(result), 3.14);
    val_release(result);
    val_release(input);
}
