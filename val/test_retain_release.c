#include "val.h"
#include "../test.h"

void test_retain_release(void) {
    /* Basic: create, retain, release twice — should not crash */
    Val *v = val_int(42);
    val_retain(v);
    ASSERT_EQ_INT(val_as_int(v), 42);
    val_release(v);
    ASSERT_EQ_INT(val_as_int(v), 42);  /* still alive, refcount 1 */
    val_release(v);  /* refcount 0, freed */

    /* val_retain returns its argument */
    Val *a = val_int(7);
    Val *b = val_retain(a);
    ASSERT_TRUE(a == b);
    val_release(a);
    val_release(b);

    /* Nested structure: list of lists */
    Val *inner_items[] = { val_int(1) };
    Val *inner = val_list(inner_items, 1);
    Val *outer_items[] = { inner };
    Val *outer = val_list(outer_items, 1);
    val_release(inner_items[0]);
    val_release(inner);
    /* inner is still alive via outer */
    ASSERT_EQ_INT(val_as_int(val_list_get(val_list_get(outer, 0), 0)), 1);
    val_release(outer);  /* frees outer and inner */
}
