#include "val.h"
#include "parse.h"
#include "../test.h"

void test_error_odd_map(void) {
    Val *input = val_string("{:a 1 :b}", 9);
    Val *result = parse(input);
    ASSERT_TYPE(result, VAL_ERROR);
    ASSERT_TRUE(strstr(val_as_error(result), "odd") != NULL);
    val_release(result);
    val_release(input);
}
