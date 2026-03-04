#include "val.h"
#include "../test.h"
#include <math.h>

void test_cmp_float_nan(void) {
    Val *nan1 = val_float(NAN);
    Val *nan2 = val_float(NAN);
    Val *one  = val_float(1.0);
    Val *neg  = val_float(-1e308);

    /* NaN == NaN */
    ASSERT_CMP_EQ(nan1, nan2);

    /* NaN sorts after all other floats */
    ASSERT_CMP_GT(nan1, one);
    ASSERT_CMP_GT(nan1, neg);
    ASSERT_CMP_LT(one, nan1);

    val_release(nan1); val_release(nan2);
    val_release(one); val_release(neg);
}
