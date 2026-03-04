# tok — Tokenizer for enkor

The `tok` concept tokenizes enkor source text into a list of token maps.

## API

```c
#include "tok.h"

Val *tok(Val *input);
```

`input` must be a `VAL_STRING` containing enkor source text.

Returns either:
- A `VAL_LIST` of `VAL_MAP` tokens on success
- A `VAL_ERROR` with a position-prefixed message on failure

The caller owns the returned Val and must release it.

## Token format

Each token is a `VAL_MAP` with four keyword keys:

| Key      | Type          | Description                          |
|----------|---------------|--------------------------------------|
| `:type`  | `VAL_KEYWORD` | Token type (see table below)         |
| `:line`  | `VAL_INT`     | 1-based line number                  |
| `:col`   | `VAL_INT`     | 1-based column number                |
| `:value` | varies        | The parsed value for this token      |

## Token types

| `:type`     | `:value` type | Examples            |
|-------------|---------------|---------------------|
| `:nil`      | `VAL_NIL`     | `nil`               |
| `:true`     | `VAL_BOOL`    | `true`              |
| `:false`    | `VAL_BOOL`    | `false`             |
| `:int`      | `VAL_INT`     | `42`, `-3`, `0`     |
| `:float`    | `VAL_FLOAT`   | `3.14`, `-0.5`      |
| `:string`   | `VAL_STRING`  | `"hello"`, `"a\nb"` |
| `:symbol`   | `VAL_SYMBOL`  | `foo`, `+`, `my-fn?`|
| `:keyword`  | `VAL_KEYWORD` | `:name`, `:type`    |
| `:lparen`   | `VAL_NIL`     | `(`                 |
| `:rparen`   | `VAL_NIL`     | `)`                 |
| `:lbrace`   | `VAL_NIL`     | `{`                 |
| `:rbrace`   | `VAL_NIL`     | `}`                 |

## Whitespace and comments

Spaces, tabs, newlines, and carriage returns are whitespace. Comments start with `;` and extend to end of line. Both are skipped between tokens.

## Error cases

The tokenizer returns `VAL_ERROR` for:
- `[` or `]` — "brackets are not supported in enkor — use () for lists"
- `,` — "commas are not valid syntax in enkor"
- Unterminated string literal
- Invalid string escape sequence (only `\"`, `\\`, `\n`, `\t` are valid)
- Bare `:` without a keyword name
- Unexpected characters

Error messages are prefixed with `line:col:` for position information.

## Example

Tokenizing `(+ 1 2.0)` produces a list of five tokens:

```
(:type :lparen  :line 1 :col 1 :value nil)
(:type :symbol  :line 1 :col 2 :value +)
(:type :int     :line 1 :col 4 :value 42)
(:type :float   :line 1 :col 6 :value 2.0)
(:type :rparen  :line 1 :col 9 :value nil)
```

## Minus disambiguation

A `-` immediately followed by a digit is a negative number. A `-` followed by a space or letter is the symbol `-`. So `-42` produces one `:int` token, while `- 42` produces a `:symbol` then an `:int`.

## Dependencies

- `val` — all inputs and outputs are Vals
