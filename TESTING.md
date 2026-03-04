# Testing System

## Structure

Each concept directory contains test files named `test_*.c`. Each file defines exactly one test function whose name matches the filename:

    test_foo.c  →  void test_foo(void)

## Writing Tests

Include your concept header first, then `test.h`:

```c
#include "val.h"
#include "../test.h"

void test_int(void) {
    Val *v = val_int(42);
    ASSERT_TYPE(v, VAL_INT);
    ASSERT_EQ_INT(val_as_int(v), 42);
    val_release(v);
}
```

## Assertion Macros

All macros print file, line, and descriptive error information to stderr before aborting.

### Generic (always available)

| Macro | Description |
|-------|-------------|
| `ASSERT_TRUE(expr)` | Expression is true. Prints the expression text on failure. |
| `ASSERT_EQ_INT(actual, expected)` | `int64_t` equality. Prints both values. |
| `ASSERT_EQ_UINT(actual, expected)` | `size_t` equality. Prints both values. |
| `ASSERT_EQ_FLOAT(actual, expected)` | `double` exact equality. Prints both values. |
| `ASSERT_EQ_STR(actual, expected)` | Null-terminated string equality. Prints both strings. |
| `ASSERT_EQ_MEM(actual, expected, n)` | Byte-level memory equality. |
| `ASSERT_NULL(ptr)` | Pointer is NULL. |
| `ASSERT_NOT_NULL(ptr)` | Pointer is not NULL. |

### Val-specific (available when `val.h` is included before `test.h`)

| Macro | Description |
|-------|-------------|
| `ASSERT_TYPE(v, type)` | `val_type(v) == type`. Prints type names on failure. |
| `ASSERT_CMP_EQ(a, b)` | `val_cmp(a, b) == 0`. Prints actual cmp result. |
| `ASSERT_CMP_LT(a, b)` | `val_cmp(a, b) < 0`. Prints actual cmp result. |
| `ASSERT_CMP_GT(a, b)` | `val_cmp(a, b) > 0`. Prints actual cmp result. |

## Expected Aborts

Test files whose name ends with `_aborts` define tests that are expected to abort:

    test_foo_aborts.c  →  void test_foo_aborts(void)

The test runner treats these inversely:
- **Normal tests:** clean exit = PASS, abort/signal = FAIL
- **`_aborts` tests:** abort/signal = PASS, clean exit = FAIL

## Test Runner

The build system generates a `test_main.c` for each concept by globbing `test_*.c` files in the concept directory. It:

1. Declares `extern void test_<name>(void)` for each test.
2. Forks a child process per test.
3. The child calls the test function.
4. The parent checks the exit status against the expectation (normal vs `_aborts`).
5. Prints each test name and PASS/FAIL to stdout.
6. Exits 0 if all tests pass, 1 otherwise.
