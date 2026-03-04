#include "val.h"
#include "../test.h"

void test_map_duplicate_key(void) {
    Val *k = val_keyword("x");
    Val *keys[] = { k, k };
    Val *vals[] = { val_int(1), val_int(2) };
    Val *m = val_map(keys, vals, 2);

    /* duplicate key: last write wins */
    ASSERT_EQ_UINT(val_len(m), 1);
    Val *result = val_map_get(m, k);
    ASSERT_NOT_NULL(result);
    ASSERT_EQ_INT(val_as_int(result), 2);

    val_release(k);
    val_release(vals[0]); val_release(vals[1]);
    val_release(m);
}
