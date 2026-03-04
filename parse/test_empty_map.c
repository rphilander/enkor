#include "val.h"
#include "parse.h"
#include "../test.h"

void test_empty_map(void) {
    Val *input = val_string("{}", 2);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);

    Val *map = val_list_get(result, 0);
    ASSERT_TYPE(map, VAL_MAP);
    ASSERT_EQ_UINT(val_len(map), 0);

    val_release(result);
    val_release(input);
}
