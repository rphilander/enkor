#include "val.h"
#include "../test.h"

void test_cmp_map(void) {
    Val *ka = val_keyword("a");
    Val *kb = val_keyword("b");
    Val *v1 = val_int(1);
    Val *v2 = val_int(2);
    Val *v3 = val_int(3);

    /* Equal maps */
    Val *mk1[] = { ka, kb }; Val *mv1[] = { v1, v2 };
    Val *m1 = val_map(mk1, mv1, 2);
    Val *m2 = val_map(mk1, mv1, 2);
    ASSERT_CMP_EQ(m1, m2);

    /* Same keys, different value */
    Val *mv2[] = { v1, v3 };
    Val *m3 = val_map(mk1, mv2, 2);
    ASSERT_TRUE(val_cmp(m1, m3) != 0);

    /* Different size: fewer entries is less */
    Val *mk3[] = { ka }; Val *mv3[] = { v1 };
    Val *m4 = val_map(mk3, mv3, 1);
    ASSERT_CMP_LT(m4, m1);

    /* Empty maps are equal */
    Val *e1 = val_map(NULL, NULL, 0);
    Val *e2 = val_map(NULL, NULL, 0);
    ASSERT_CMP_EQ(e1, e2);

    val_release(ka); val_release(kb);
    val_release(v1); val_release(v2); val_release(v3);
    val_release(m1); val_release(m2);
    val_release(m3); val_release(m4);
    val_release(e1); val_release(e2);
}
