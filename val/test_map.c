#include "val.h"
#include "../test.h"

void test_map(void) {
    Val *keys[] = { val_keyword("x"), val_keyword("y") };
    Val *vals[] = { val_int(10), val_int(20) };
    Val *m = val_map(keys, vals, 2);
    ASSERT_TYPE(m, VAL_MAP);
    ASSERT_EQ_UINT(val_len(m), 2);
    val_release(keys[0]); val_release(keys[1]);
    val_release(vals[0]); val_release(vals[1]);
    val_release(m);

    /* empty map */
    Val *empty = val_map(NULL, NULL, 0);
    ASSERT_TYPE(empty, VAL_MAP);
    ASSERT_EQ_UINT(val_len(empty), 0);
    val_release(empty);
}
