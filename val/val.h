#ifndef VAL_H
#define VAL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    VAL_NIL,
    VAL_BOOL,
    VAL_INT,
    VAL_FLOAT,
    VAL_STRING,
    VAL_SYMBOL,
    VAL_KEYWORD,
    VAL_LIST,
    VAL_MAP,
    VAL_ERROR,
    VAL_BUILTIN,
    VAL_EFFECT,
} ValType;

typedef struct Val Val;

/* Function pointer type for builtin functions */
typedef Val *(*BuiltinFn)(Val **args, size_t argc);

/* Constructors */
Val *val_nil(void);
Val *val_bool(bool b);
Val *val_int(int64_t n);
Val *val_float(double d);
Val *val_string(const char *data, size_t len);
Val *val_symbol(const char *name);
Val *val_keyword(const char *name);
Val *val_list(Val **items, size_t len);
Val *val_map(Val **keys, Val **vals, size_t len);
Val *val_error(const char *message);
Val *val_builtin(const char *name, BuiltinFn fn);
Val *val_effect(const char *name);

/* Memory management */
Val *val_retain(Val *v);
void val_release(Val *v);

/* Inspection */
ValType val_type(const Val *v);
int val_cmp(const Val *a, const Val *b);
uint64_t val_hash(const Val *v);

/*
 * Accessors — abort on type mismatch.
 * Returned pointers are non-owning and valid for the lifetime of the Val.
 */
bool val_as_bool(const Val *v);
int64_t val_as_int(const Val *v);
double val_as_float(const Val *v);
const char *val_as_string(const Val *v, size_t *len);
const char *val_as_symbol(const Val *v);
const char *val_as_keyword(const Val *v);
const char *val_as_error(const Val *v);
const char *val_as_builtin_name(const Val *v);
BuiltinFn val_as_builtin(const Val *v);
const char *val_as_effect(const Val *v);

/*
 * Collections.
 * val_len works on STRING (byte length), LIST, and MAP. Aborts otherwise.
 * val_list_get and val_map_get return non-owning pointers.
 * val_map_get returns NULL if the key is not found.
 */
size_t val_len(const Val *v);
Val *val_list_get(const Val *v, size_t index);
Val *val_map_get(const Val *map, const Val *key);

#endif
