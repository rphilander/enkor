#include "val.h"
#include "../test.h"

void test_len(void) {
    Val *s = val_string("hello", 5);
    ASSERT_EQ_UINT(val_len(s), 5);
    val_release(s);

    Val *empty_s = val_string("", 0);
    ASSERT_EQ_UINT(val_len(empty_s), 0);
    val_release(empty_s);

    Val *items[] = { val_int(1), val_int(2) };
    Val *lst = val_list(items, 2);
    ASSERT_EQ_UINT(val_len(lst), 2);
    val_release(items[0]); val_release(items[1]);
    val_release(lst);

    Val *k = val_keyword("a");
    Val *v = val_int(1);
    Val *m = val_map(&k, &v, 1);
    ASSERT_EQ_UINT(val_len(m), 1);
    val_release(k); val_release(v); val_release(m);
}
