#include "val.h"
#include "../test.h"

void test_hash_map_order(void) {
    /* Maps with same entries in different insertion order must hash equally */
    Val *ka = val_keyword("a");
    Val *kb = val_keyword("b");
    Val *v1 = val_int(1);
    Val *v2 = val_int(2);

    Val *fwd_k[] = { ka, kb };
    Val *fwd_v[] = { v1, v2 };
    Val *rev_k[] = { kb, ka };
    Val *rev_v[] = { v2, v1 };

    Val *m1 = val_map(fwd_k, fwd_v, 2);
    Val *m2 = val_map(rev_k, rev_v, 2);

    ASSERT_CMP_EQ(m1, m2);
    ASSERT_EQ_UINT(val_hash(m1), val_hash(m2));

    val_release(ka); val_release(kb);
    val_release(v1); val_release(v2);
    val_release(m1); val_release(m2);
}
