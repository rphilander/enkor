#include "val.h"
#include "json.h"
#include "../test.h"

void test_nested(void) {
    Val *input = val_string("{\"items\": [1, 2], \"ok\": true}", 29);
    Val *result = json_to_val(input);
    ASSERT_NOT_ERROR(result);
    ASSERT_TYPE(result, VAL_MAP);

    Val *k = val_string("items", 5);
    Val *items = val_map_get(result, k);
    ASSERT_NOT_NULL(items);
    ASSERT_TYPE(items, VAL_LIST);
    ASSERT_EQ_UINT(val_len(items), 2);

    Val *k2 = val_string("ok", 2);
    Val *ok = val_map_get(result, k2);
    ASSERT_TYPE(ok, VAL_BOOL);
    ASSERT_TRUE(val_as_bool(ok) == true);

    val_release(k); val_release(k2);
    val_release(result); val_release(input);
}
