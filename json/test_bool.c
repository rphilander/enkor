#include "val.h"
#include "json.h"
#include "../test.h"

void test_bool(void) {
    Val *t = val_string("true", 4);
    Val *f = val_string("false", 5);
    Val *rt = json_to_val(t);
    Val *rf = json_to_val(f);
    ASSERT_TYPE(rt, VAL_BOOL);
    ASSERT_TRUE(val_as_bool(rt) == true);
    ASSERT_TYPE(rf, VAL_BOOL);
    ASSERT_TRUE(val_as_bool(rf) == false);
    val_release(rt); val_release(rf);
    val_release(t); val_release(f);
}
