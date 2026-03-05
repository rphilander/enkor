#ifndef TEST_H
#define TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define ASSERT_TRUE(expr) do { \
    if (!(expr)) { \
        fprintf(stderr, "FAIL: %s:%d: expected true: %s\n", \
                __FILE__, __LINE__, #expr); \
        abort(); \
    } \
} while(0)

#define ASSERT_EQ_INT(actual, expected) do { \
    int64_t _a = (actual), _e = (expected); \
    if (_a != _e) { \
        fprintf(stderr, "FAIL: %s:%d: expected %lld, got %lld\n", \
                __FILE__, __LINE__, (long long)_e, (long long)_a); \
        abort(); \
    } \
} while(0)

#define ASSERT_EQ_UINT(actual, expected) do { \
    size_t _a = (actual), _e = (expected); \
    if (_a != _e) { \
        fprintf(stderr, "FAIL: %s:%d: expected %zu, got %zu\n", \
                __FILE__, __LINE__, _e, _a); \
        abort(); \
    } \
} while(0)

#define ASSERT_EQ_FLOAT(actual, expected) do { \
    double _a = (actual), _e = (expected); \
    if (_a != _e) { \
        fprintf(stderr, "FAIL: %s:%d: expected %g, got %g\n", \
                __FILE__, __LINE__, _e, _a); \
        abort(); \
    } \
} while(0)

#define ASSERT_EQ_STR(actual, expected) do { \
    const char *_a = (actual), *_e = (expected); \
    if (strcmp(_a, _e) != 0) { \
        fprintf(stderr, "FAIL: %s:%d: expected \"%s\", got \"%s\"\n", \
                __FILE__, __LINE__, _e, _a); \
        abort(); \
    } \
} while(0)

#define ASSERT_EQ_MEM(actual, expected, n) do { \
    if (memcmp((actual), (expected), (n)) != 0) { \
        fprintf(stderr, "FAIL: %s:%d: memory comparison failed (%zu bytes)\n", \
                __FILE__, __LINE__, (size_t)(n)); \
        abort(); \
    } \
} while(0)

#define ASSERT_NULL(ptr) do { \
    if ((ptr) != NULL) { \
        fprintf(stderr, "FAIL: %s:%d: expected NULL: %s\n", \
                __FILE__, __LINE__, #ptr); \
        abort(); \
    } \
} while(0)

#define ASSERT_NOT_NULL(ptr) do { \
    if ((ptr) == NULL) { \
        fprintf(stderr, "FAIL: %s:%d: unexpected NULL: %s\n", \
                __FILE__, __LINE__, #ptr); \
        abort(); \
    } \
} while(0)

/* Val-specific macros, available when val.h is included first */
#ifdef VAL_H

static inline const char *_test_type_name(ValType t) {
    switch (t) {
    case VAL_NIL:     return "NIL";
    case VAL_BOOL:    return "BOOL";
    case VAL_INT:     return "INT";
    case VAL_FLOAT:   return "FLOAT";
    case VAL_STRING:  return "STRING";
    case VAL_SYMBOL:  return "SYMBOL";
    case VAL_KEYWORD: return "KEYWORD";
    case VAL_LIST:    return "LIST";
    case VAL_MAP:     return "MAP";
    case VAL_ERROR:   return "ERROR";
    case VAL_BUILTIN: return "BUILTIN";
    case VAL_EFFECT:  return "EFFECT";
    }
    return "???";
}

#define ASSERT_TYPE(v, expected) do { \
    ValType _a = val_type(v), _e = (expected); \
    if (_a != _e) { \
        fprintf(stderr, "FAIL: %s:%d: expected type %s, got %s\n", \
                __FILE__, __LINE__, _test_type_name(_e), \
                _test_type_name(_a)); \
        abort(); \
    } \
} while(0)

#define ASSERT_CMP_EQ(a, b) do { \
    int _c = val_cmp((a), (b)); \
    if (_c != 0) { \
        fprintf(stderr, "FAIL: %s:%d: expected cmp == 0, got %d\n", \
                __FILE__, __LINE__, _c); \
        abort(); \
    } \
} while(0)

#define ASSERT_CMP_LT(a, b) do { \
    int _c = val_cmp((a), (b)); \
    if (_c >= 0) { \
        fprintf(stderr, "FAIL: %s:%d: expected cmp < 0, got %d\n", \
                __FILE__, __LINE__, _c); \
        abort(); \
    } \
} while(0)

#define ASSERT_CMP_GT(a, b) do { \
    int _c = val_cmp((a), (b)); \
    if (_c <= 0) { \
        fprintf(stderr, "FAIL: %s:%d: expected cmp > 0, got %d\n", \
                __FILE__, __LINE__, _c); \
        abort(); \
    } \
} while(0)

#endif /* VAL_H */

#endif /* TEST_H */
