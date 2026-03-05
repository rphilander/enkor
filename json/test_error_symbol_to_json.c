#include "val.h"
#include "json.h"
#include "../test.h"

void test_error_symbol_to_json(void) {
    Val *sym = val_symbol("foo");
    Val *result = val_to_json(sym);
    ASSERT_TYPE(result, VAL_ERROR);
    val_release(result);
    val_release(sym);
}
