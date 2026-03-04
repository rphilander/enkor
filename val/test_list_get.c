#include "val.h"
#include "../test.h"

void test_list_get(void) {
    Val *items[] = { val_string("a", 1), val_int(2), val_bool(true) };
    Val *lst = val_list(items, 3);

    ASSERT_TYPE(val_list_get(lst, 0), VAL_STRING);
    ASSERT_TYPE(val_list_get(lst, 1), VAL_INT);
    ASSERT_EQ_INT(val_as_int(val_list_get(lst, 1)), 2);
    ASSERT_TRUE(val_as_bool(val_list_get(lst, 2)));

    val_release(items[0]); val_release(items[1]); val_release(items[2]);
    val_release(lst);
}
