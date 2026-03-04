#include "val.h"
#include "../test.h"

void test_cmp_cross_type(void) {
    Val *n   = val_nil();
    Val *b   = val_bool(false);
    Val *i   = val_int(0);
    Val *f   = val_float(0.0);
    Val *s   = val_string("", 0);
    Val *sym = val_symbol("a");
    Val *kw  = val_keyword("a");
    Val *lst = val_list(NULL, 0);
    Val *m   = val_map(NULL, NULL, 0);

    /* Strict ordering: NIL < BOOL < INT < FLOAT < STRING < SYMBOL < KEYWORD < LIST < MAP */
    ASSERT_CMP_LT(n, b);
    ASSERT_CMP_LT(b, i);
    ASSERT_CMP_LT(i, f);
    ASSERT_CMP_LT(f, s);
    ASSERT_CMP_LT(s, sym);
    ASSERT_CMP_LT(sym, kw);
    ASSERT_CMP_LT(kw, lst);
    ASSERT_CMP_LT(lst, m);

    /* Reverse */
    ASSERT_CMP_GT(m, n);

    val_release(n); val_release(b); val_release(i);
    val_release(f); val_release(s); val_release(sym);
    val_release(kw); val_release(lst); val_release(m);
}
