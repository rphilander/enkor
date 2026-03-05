#include "val.h"
#include "json.h"
#include "../test.h"

void test_null(void) {
    Val *input = val_string("null", 4);
    Val *result = json_to_val(input);
    ASSERT_NOT_ERROR(result);
    ASSERT_TYPE(result, VAL_NIL);
    val_release(result);
    val_release(input);
}
