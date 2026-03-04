#include "val.h"
#include "../test.h"
#include <math.h>

void test_hash_float_nan(void) {
    Val *a = val_float(NAN);
    Val *b = val_float(NAN);
    ASSERT_EQ_UINT(val_hash(a), val_hash(b));
    val_release(a); val_release(b);
}
