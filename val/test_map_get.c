#include "val.h"
#include "../test.h"

void test_map_get(void) {
    Val *keys[] = { val_keyword("x"), val_keyword("y") };
    Val *vals[] = { val_int(10), val_int(20) };
    Val *m = val_map(keys, vals, 2);

    Val *result = val_map_get(m, keys[0]);
    ASSERT_NOT_NULL(result);
    ASSERT_EQ_INT(val_as_int(result), 10);

    result = val_map_get(m, keys[1]);
    ASSERT_NOT_NULL(result);
    ASSERT_EQ_INT(val_as_int(result), 20);

    val_release(keys[0]); val_release(keys[1]);
    val_release(vals[0]); val_release(vals[1]);
    val_release(m);
}
