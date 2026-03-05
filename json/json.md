# json — Bidirectional JSON-Val conversion

The `json` concept provides isomorphic conversion between JSON text and Val structures.

## API

```c
#include "json.h"

Val *json_to_val(Val *json_str);  // JSON string → Val or VAL_ERROR
Val *val_to_json(Val *v);         // Val → JSON string or VAL_ERROR
```

Both functions return owned Vals. The caller must release them.

## JSON → Val mapping

| JSON      | Val type     |
|-----------|-------------|
| `null`    | `VAL_NIL`   |
| `true`    | `VAL_BOOL`  |
| `false`   | `VAL_BOOL`  |
| integer   | `VAL_INT`   |
| float     | `VAL_FLOAT` |
| string    | `VAL_STRING`|
| array     | `VAL_LIST`  |
| object    | `VAL_MAP`   |

JSON numbers that are whole numbers (within safe integer range) become `VAL_INT`. Others become `VAL_FLOAT`. Object keys become `VAL_STRING` keys in the map.

## Val → JSON mapping

The same mapping in reverse. Only JSON-representable types are supported:
- `VAL_NIL`, `VAL_BOOL`, `VAL_INT`, `VAL_FLOAT`, `VAL_STRING`, `VAL_LIST`, `VAL_MAP`

Returns `VAL_ERROR` for:
- Symbols, keywords, builtins, effects, errors
- Maps with non-string keys

## Round-trip guarantee

`json_to_val(val_to_json(v))` produces a Val equal to `v` for all supported types. `val_to_json(json_to_val(s))` produces semantically equivalent JSON.

## Implementation

Uses [cJSON](https://github.com/DaveGamble/cJSON) (MIT licensed, vendored in `vendor/`).

## Dependencies

- `val` — all inputs and outputs are Vals
