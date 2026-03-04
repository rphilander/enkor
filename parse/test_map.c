#include "val.h"
#include "parse.h"
#include "../test.h"

void test_map(void) {
    Val *input = val_string("{:a 1 :b 2}", 11);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);

    Val *map = val_list_get(result, 0);
    ASSERT_TYPE(map, VAL_MAP);
    ASSERT_EQ_UINT(val_len(map), 2);

    Val *ka = val_keyword("a");
    Val *va = val_map_get(map, ka);
    ASSERT_NOT_NULL(va);
    ASSERT_EQ_INT(val_as_int(va), 1);

    Val *kb = val_keyword("b");
    Val *vb = val_map_get(map, kb);
    ASSERT_NOT_NULL(vb);
    ASSERT_EQ_INT(val_as_int(vb), 2);

    val_release(ka);
    val_release(kb);
    val_release(result);
    val_release(input);
}
