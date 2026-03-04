#include "val.h"
#include "parse.h"
#include "../test.h"

void test_map_in_list(void) {
    Val *input = val_string("(get {:a 1} :a)", 15);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_LIST);
    ASSERT_EQ_UINT(val_len(result), 1);

    Val *list = val_list_get(result, 0);
    ASSERT_TYPE(list, VAL_LIST);
    ASSERT_EQ_UINT(val_len(list), 3);
    ASSERT_EQ_STR(val_as_symbol(val_list_get(list, 0)), "get");

    Val *map = val_list_get(list, 1);
    ASSERT_TYPE(map, VAL_MAP);

    Val *ka = val_keyword("a");
    ASSERT_EQ_INT(val_as_int(val_map_get(map, ka)), 1);
    val_release(ka);

    ASSERT_TYPE(val_list_get(list, 2), VAL_KEYWORD);

    val_release(result);
    val_release(input);
}
