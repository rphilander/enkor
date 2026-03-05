#include "val.h"
#include "json.h"
#include "../test.h"

void test_error_invalid_json(void) {
    Val *input = val_string("{bad json", 9);
    Val *result = json_to_val(input);
    ASSERT_TYPE(result, VAL_ERROR);
    val_release(result);
    val_release(input);
}
