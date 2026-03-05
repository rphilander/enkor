#include "val.h"
#include "json.h"
#include "../test.h"

void test_array(void) {
    Val *input = val_string("[1, 2, 3]", 9);
    Val *result = json_to_val(input);
    ASSERT_NOT_ERROR(result);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 3);
    ASSERT_EQ_INT(val_as_int(val_list_get(result, 0)), 1);
    ASSERT_EQ_INT(val_as_int(val_list_get(result, 1)), 2);
    ASSERT_EQ_INT(val_as_int(val_list_get(result, 2)), 3);
    val_release(result);
    val_release(input);
}
