#include "val.h"
#include "../test.h"

void test_cmp_list(void) {
    Val *a_items[] = { val_int(1), val_int(2) };
    Val *b_items[] = { val_int(1), val_int(3) };
    Val *c_items[] = { val_int(1) };

    Val *a = val_list(a_items, 2);
    Val *b = val_list(b_items, 2);
    Val *c = val_list(c_items, 1);

    ASSERT_CMP_LT(a, b);       /* [1,2] < [1,3] */
    ASSERT_CMP_LT(c, a);       /* [1] < [1,2] (prefix) */

    Val *a2 = val_list(a_items, 2);
    ASSERT_CMP_EQ(a, a2);

    Val *empty = val_list(NULL, 0);
    ASSERT_CMP_LT(empty, c);   /* [] < [1] */

    val_release(a_items[0]); val_release(a_items[1]);
    val_release(b_items[0]); val_release(b_items[1]);
    val_release(c_items[0]);
    val_release(a); val_release(b);
    val_release(c); val_release(a2);
    val_release(empty);
}
