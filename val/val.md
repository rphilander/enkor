# val — Core Value Type

All data in enkor is represented by `Val`, an opaque, immutable, reference-counted type. You always work with `Val *` pointers and never access struct fields directly.

## Types

There are 10 value types, represented by the `ValType` enum:

```c
VAL_NIL, VAL_BOOL, VAL_INT, VAL_FLOAT,
VAL_STRING, VAL_SYMBOL, VAL_KEYWORD,
VAL_LIST, VAL_MAP, VAL_ERROR
```

Check a value's type with `val_type(v)`:

```c
if (val_type(v) == VAL_INT) {
    int64_t n = val_as_int(v);
}
```

## Creating Values

Each type has a constructor that returns a new `Val *` with refcount 1:

```c
Val *n = val_nil();
Val *b = val_bool(true);
Val *i = val_int(42);
Val *f = val_float(3.14);
Val *s = val_string("hello", 5);    /* data + byte length */
Val *sym = val_symbol("foo");        /* null-terminated name */
Val *kw = val_keyword("bar");        /* null-terminated name */
Val *e = val_error("something went wrong");  /* error message */
```

Lists take an array of existing `Val *` pointers. The list retains each element:

```c
Val *items[] = { val_int(1), val_int(2), val_int(3) };
Val *lst = val_list(items, 3);
val_release(items[0]);  /* list holds its own references */
val_release(items[1]);
val_release(items[2]);
```

Maps take parallel arrays of keys and values. Any `Val` can be a key. Duplicate keys are resolved as last-write-wins:

```c
Val *keys[] = { val_keyword("x"), val_keyword("y") };
Val *vals[] = { val_int(10), val_int(20) };
Val *m = val_map(keys, vals, 2);
val_release(keys[0]); val_release(keys[1]);
val_release(vals[0]); val_release(vals[1]);
```

## Memory Management

Values are reference-counted. Every constructor returns a value with refcount 1. Use `val_retain` and `val_release` to manage ownership:

```c
Val *v = val_int(42);   /* refcount = 1 */
val_retain(v);          /* refcount = 2 */
val_release(v);         /* refcount = 1 */
val_release(v);         /* refcount = 0, freed */
```

`val_retain` returns its argument for convenience. Both functions are NULL-safe.

When a value is freed, it recursively releases its children (list elements, map keys and values). Because values are immutable, reference cycles cannot form, so reference counting is always correct.

**Ownership rules:**
- If you create it, you own it (refcount 1).
- If you want to keep a reference beyond the lifetime of its owner, call `val_retain`.
- When you're done with a reference, call `val_release`.
- `val_list_get` and `val_map_get` return non-owning pointers. The returned value is valid as long as the container is alive. Retain it if you need it longer.

## Extracting Values

Use the `val_as_*` accessors to extract the underlying data. These **abort the process** if the type doesn't match, so always check `val_type` first:

```c
bool val_as_bool(const Val *v);
int64_t val_as_int(const Val *v);
double val_as_float(const Val *v);
const char *val_as_string(const Val *v, size_t *len);  /* writes byte length to *len */
const char *val_as_symbol(const Val *v);                /* null-terminated */
const char *val_as_keyword(const Val *v);               /* null-terminated */
const char *val_as_error(const Val *v);                 /* null-terminated */
```

Returned `const char *` pointers are non-owning. Do not free them.

## Strings

Strings are opaque UTF-8 byte sequences. They are not arrays of characters — there is no indexing into strings and no concept of "character." Strings may contain embedded null bytes; their length is tracked explicitly. `val_as_string` returns both the data pointer and the byte length.

Symbols and keywords are identifiers (following Clojure naming rules) and are always valid null-terminated C strings.

## Collections

```c
size_t val_len(const Val *v);
```

Returns the byte length of a string, element count of a list, or entry count of a map. Aborts on other types.

```c
Val *val_list_get(const Val *v, size_t index);
```

Returns the element at `index`. Aborts if out of bounds.

```c
Val *val_map_get(const Val *map, const Val *key);
```

Returns the value associated with `key`, or `NULL` if the key is not present.

## Comparison and Hashing

```c
int val_cmp(const Val *a, const Val *b);
uint64_t val_hash(const Val *v);
```

`val_cmp` defines a total order over all values. It returns negative, zero, or positive. Two values are equal if and only if `val_cmp` returns 0.

**Cross-type ordering:** Types are ordered by their enum value: NIL < BOOL < INT < FLOAT < STRING < SYMBOL < KEYWORD < LIST < MAP < ERROR. Values of different types are never equal.

**Within-type ordering:**
- NIL: all nils are equal.
- BOOL: false < true.
- INT: numeric.
- FLOAT: numeric. NaN == NaN, NaN sorts after all other floats. -0.0 == +0.0.
- STRING, SYMBOL, KEYWORD: byte-level lexicographic.
- LIST: lexicographic element-by-element. Shorter list < longer if one is a prefix of the other.
- MAP: compare by length first, then by sorted entries (key first, then value).

**Hash contract:** Equal values always have equal hashes. The hash is precomputed at creation time and returned by `val_hash`.

## Errors

Errors are values that carry an error message string. They are the runtime representation of exceptions and are returned by functions that can fail (e.g. parsing invalid input).

```c
Val *e = val_error("unexpected end of input");
const char *msg = val_as_error(e);  /* "unexpected end of input" */
```

Check for errors with `val_type(v) == VAL_ERROR`. Errors participate in comparison and hashing like all other values — two errors with the same message are equal, and errors sort after MAP in the type ordering.
