#include "val.h"
#include "../test.h"

void test_hash_equal_values(void) {
    /* Equal ints */
    Val *a = val_int(42);
    Val *b = val_int(42);
    ASSERT_CMP_EQ(a, b);
    ASSERT_EQ_UINT(val_hash(a), val_hash(b));
    val_release(a); val_release(b);

    /* Equal strings */
    Val *s1 = val_string("hello", 5);
    Val *s2 = val_string("hello", 5);
    ASSERT_EQ_UINT(val_hash(s1), val_hash(s2));
    val_release(s1); val_release(s2);

    /* Equal lists */
    Val *items1[] = { val_int(1), val_int(2) };
    Val *items2[] = { val_int(1), val_int(2) };
    Val *l1 = val_list(items1, 2);
    Val *l2 = val_list(items2, 2);
    ASSERT_EQ_UINT(val_hash(l1), val_hash(l2));
    val_release(items1[0]); val_release(items1[1]);
    val_release(items2[0]); val_release(items2[1]);
    val_release(l1); val_release(l2);
}
