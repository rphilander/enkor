#include "val.h"
#include "../test.h"

void test_map_get_missing(void) {
    Val *k = val_keyword("x");
    Val *v = val_int(1);
    Val *m = val_map(&k, &v, 1);

    Val *missing = val_keyword("y");
    ASSERT_NULL(val_map_get(m, missing));

    /* Also test empty map */
    Val *empty = val_map(NULL, NULL, 0);
    ASSERT_NULL(val_map_get(empty, k));

    val_release(k); val_release(v);
    val_release(m); val_release(missing);
    val_release(empty);
}
