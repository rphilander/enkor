#include "val.h"
#include "../test.h"

void test_list(void) {
    Val *items[] = { val_int(10), val_int(20), val_int(30) };
    Val *v = val_list(items, 3);
    ASSERT_TYPE(v, VAL_LIST);
    ASSERT_EQ_UINT(val_len(v), 3);
    ASSERT_EQ_INT(val_as_int(val_list_get(v, 0)), 10);
    ASSERT_EQ_INT(val_as_int(val_list_get(v, 1)), 20);
    ASSERT_EQ_INT(val_as_int(val_list_get(v, 2)), 30);
    val_release(items[0]);
    val_release(items[1]);
    val_release(items[2]);
    val_release(v);

    /* empty list */
    Val *empty = val_list(NULL, 0);
    ASSERT_TYPE(empty, VAL_LIST);
    ASSERT_EQ_UINT(val_len(empty), 0);
    val_release(empty);
}
