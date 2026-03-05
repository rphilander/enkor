#include "val.h"
#include "json.h"
#include "../test.h"

void test_roundtrip_collections(void) {
    /* [1, 2] → json → [1, 2] */
    Val *items[2] = { val_int(1), val_int(2) };
    Val *list = val_list(items, 2);
    Val *lj = val_to_json(list);
    ASSERT_NOT_ERROR(lj);
    Val *l2 = json_to_val(lj);
    ASSERT_TYPE(l2, VAL_LIST);
    ASSERT_EQ_UINT(val_len(l2), 2);
    ASSERT_EQ_INT(val_as_int(val_list_get(l2, 0)), 1);
    ASSERT_EQ_INT(val_as_int(val_list_get(l2, 1)), 2);

    /* {"a": 1} → json → {"a": 1} */
    Val *k = val_string("a", 1);
    Val *v = val_int(1);
    Val *map = val_map(&k, &v, 1);
    Val *mj = val_to_json(map);
    ASSERT_NOT_ERROR(mj);
    Val *m2 = json_to_val(mj);
    ASSERT_TYPE(m2, VAL_MAP);
    Val *v2 = val_map_get(m2, k);
    ASSERT_NOT_NULL(v2);
    ASSERT_EQ_INT(val_as_int(v2), 1);

    val_release(items[0]); val_release(items[1]);
    val_release(list); val_release(lj); val_release(l2);
    val_release(k); val_release(v);
    val_release(map); val_release(mj); val_release(m2);
}
