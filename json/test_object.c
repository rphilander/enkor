#include "val.h"
#include "json.h"
#include "../test.h"

void test_object(void) {
    Val *input = val_string("{\"a\": 1, \"b\": 2}", 16);
    Val *result = json_to_val(input);
    ASSERT_NOT_ERROR(result);
    ASSERT_TYPE(result, VAL_MAP);
    ASSERT_EQ_UINT(val_len(result), 2);

    Val *ka = val_string("a", 1);
    Val *va = val_map_get(result, ka);
    ASSERT_NOT_NULL(va);
    ASSERT_EQ_INT(val_as_int(va), 1);

    Val *kb = val_string("b", 1);
    Val *vb = val_map_get(result, kb);
    ASSERT_NOT_NULL(vb);
    ASSERT_EQ_INT(val_as_int(vb), 2);

    val_release(ka); val_release(kb);
    val_release(result);
    val_release(input);
}
