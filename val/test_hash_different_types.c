#include "val.h"
#include "../test.h"

void test_hash_different_types(void) {
    /* Same underlying name, different types — must have different hashes */
    Val *s   = val_string("foo", 3);
    Val *sym = val_symbol("foo");
    Val *kw  = val_keyword("foo");

    ASSERT_TRUE(val_hash(s) != val_hash(sym));
    ASSERT_TRUE(val_hash(sym) != val_hash(kw));
    ASSERT_TRUE(val_hash(s) != val_hash(kw));

    val_release(s); val_release(sym); val_release(kw);
}
