#include "val.h"
#include "json.h"
#include "../test.h"

void test_roundtrip_primitives(void) {
    /* nil → "null" → nil */
    Val *n = val_nil();
    Val *nj = val_to_json(n);
    ASSERT_NOT_ERROR(nj);
    Val *n2 = json_to_val(nj);
    ASSERT_TYPE(n2, VAL_NIL);

    /* true → "true" → true */
    Val *b = val_bool(true);
    Val *bj = val_to_json(b);
    ASSERT_NOT_ERROR(bj);
    Val *b2 = json_to_val(bj);
    ASSERT_TYPE(b2, VAL_BOOL);
    ASSERT_TRUE(val_as_bool(b2) == true);

    /* 42 → "42" → 42 */
    Val *i = val_int(42);
    Val *ij = val_to_json(i);
    ASSERT_NOT_ERROR(ij);
    Val *i2 = json_to_val(ij);
    ASSERT_TYPE(i2, VAL_INT);
    ASSERT_EQ_INT(val_as_int(i2), 42);

    /* "hello" → "\"hello\"" → "hello" */
    Val *s = val_string("hello", 5);
    Val *sj = val_to_json(s);
    ASSERT_NOT_ERROR(sj);
    Val *s2 = json_to_val(sj);
    ASSERT_TYPE(s2, VAL_STRING);
    size_t len;
    ASSERT_EQ_STR(val_as_string(s2, &len), "hello");

    val_release(n); val_release(nj); val_release(n2);
    val_release(b); val_release(bj); val_release(b2);
    val_release(i); val_release(ij); val_release(i2);
    val_release(s); val_release(sj); val_release(s2);
}
