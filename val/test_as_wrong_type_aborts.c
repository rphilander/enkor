#include "val.h"
#include "../test.h"

void test_as_wrong_type_aborts(void) {
    Val *v = val_nil();
    val_as_int(v);   /* should abort: nil is not an int */
    val_release(v);  /* should never reach here */
}
