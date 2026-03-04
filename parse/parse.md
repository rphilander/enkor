# parse — Parser for enkor

The `parse` concept converts enkor source text into Val structures.

## API

```c
#include "parse.h"

Val *parse(Val *input);
```

`input` must be a `VAL_STRING` containing enkor source text.

Returns either:
- A `VAL_LIST` of top-level forms on success
- A `VAL_ERROR` with a position-prefixed message on failure

The caller owns the returned Val and must release it.

## How it works

1. Tokenizes the input by calling `tok()` internally
2. Walks the token list with a recursive descent parser
3. Atoms (nil, bool, int, float, string, symbol, keyword) become their corresponding Val types directly
4. `(...)` becomes a `VAL_LIST`
5. `{...}` becomes a `VAL_MAP`
6. Nesting is handled recursively

## Examples

```
""           → ()                  ; empty list of forms
"42"         → (42)                ; list containing one int
"(+ 1 2)"   → ((+ 1 2))           ; list containing one list
"{:a 1}"     → ({:a 1})            ; list containing one map
"1 2 3"      → (1 2 3)            ; three top-level forms
```

## Error cases

Returns `VAL_ERROR` for:
- **Tokenizer errors** — forwarded directly (brackets, commas, bad escapes, etc.)
- **Unmatched `(`** — open paren without closing paren
- **Unmatched `{`** — open brace without closing brace
- **Unexpected `)`** — close paren at top level or inside a map
- **Unexpected `}`** — close brace at top level or inside a list
- **Odd map count** — `{:a 1 :b}` has 3 forms, maps require even count

All error messages include `line:col:` position prefixes.

## Dependencies

- `val` — all inputs and outputs are Vals
- `tok` — tokenization is done internally
