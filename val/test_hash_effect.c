#include "val.h"
#include "../test.h"

void test_hash_effect(void) {
    Val *a = val_effect("def");
    Val *b = val_effect("def");
    Val *c = val_effect("time-now");

    /* same name → same hash */
    ASSERT_TRUE(val_hash(a) == val_hash(b));
    /* different name → different hash */
    ASSERT_TRUE(val_hash(a) != val_hash(c));

    /* effect and symbol with same name → different hash (type is mixed in) */
    Val *sym = val_symbol("def");
    ASSERT_TRUE(val_hash(a) != val_hash(sym));

    val_release(sym);
    val_release(a);
    val_release(b);
    val_release(c);
}
