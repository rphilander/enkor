#include "val.h"
#include "../test.h"

static Val *test_add(Val **args, size_t argc) {
    if (argc != 2) return val_error("expected 2 args");
    return val_int(val_as_int(args[0]) + val_as_int(args[1]));
}

void test_builtin_call(void) {
    Val *b = val_builtin("+", test_add);
    BuiltinFn fn = val_as_builtin(b);

    Val *a1 = val_int(3);
    Val *a2 = val_int(4);
    Val *args[] = { a1, a2 };
    Val *result = fn(args, 2);

    ASSERT_TYPE(result, VAL_INT);
    ASSERT_EQ_INT(val_as_int(result), 7);

    val_release(result);
    val_release(a1);
    val_release(a2);
    val_release(b);
}
