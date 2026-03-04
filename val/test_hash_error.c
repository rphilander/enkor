#include "val.h"
#include "../test.h"

void test_hash_error(void) {
    Val *a = val_error("oops");
    Val *b = val_error("oops");
    ASSERT_EQ_UINT(val_hash(a), val_hash(b));

    /* Error and string with same text have different hashes */
    Val *s = val_string("oops", 4);
    ASSERT_TRUE(val_hash(a) != val_hash(s));

    val_release(a); val_release(b); val_release(s);
}
