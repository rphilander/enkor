# step — Step evaluator for enkor

The `step` concept implements a single-step evaluator for enkor expressions. Each call to `step()` advances evaluation by one small step, returning a new immutable state. This design enables concurrency (interleave steps from different computations), tracing (inspect state between steps), and security (check permissions before each step).

## API

```c
#include "step.h"

Val *step(Val *state);
Val *step_init(Val *expr, Val *env);
```

`step_init` creates an initial state from an expression and environment. `step` takes a state and returns either a new state (a `VAL_MAP`) or a `VAL_ERROR`.

The caller owns the returned Val and must release it.

## State structure

The state is a `VAL_MAP` with these keys:

| Key       | Type          | Description                              |
|-----------|---------------|------------------------------------------|
| `:expr`   | any Val       | Current expression / result when done    |
| `:env`    | `VAL_LIST`    | Environment: list of maps, inner to outer|
| `:stack`  | `VAL_LIST`    | Continuation frames                      |
| `:status` | `VAL_KEYWORD` | `:running`, `:done`, or `:suspended`     |
| `:effect` | `VAL_MAP`     | Effect request (only when `:suspended`)  |

## Running to completion

```c
Val *state = step_init(expr, env);
while (1) {
    Val *status = val_map_get(state, kw_status);
    if (val_cmp(status, kw_done) == 0) break;
    Val *next = step(state);
    val_release(state);
    state = next;
    if (val_type(state) == VAL_ERROR) { /* handle error */ break; }
}
Val *result = val_map_get(state, kw_expr);
```

## Special forms

These are wired into the stepper:

| Form                    | Description                              |
|-------------------------|------------------------------------------|
| `(if cond then else)`   | Conditional — nil and false are falsy    |
| `(fn (params...) body)` | Create a closure capturing current env   |
| `(let (bindings...) body)` | Local bindings, pairs of name/value   |
| `(do expr1 expr2 ...)`  | Sequence, returns last value             |
| `(quote expr)`          | Return expression unevaluated            |
| `(loop (bindings...) body)` | Iteration with `recur`              |
| `(recur arg1 arg2 ...)`    | Jump back to enclosing `loop`        |

## Environment

The environment is a `VAL_LIST` of `VAL_MAP`s, ordered from innermost to outermost scope. Symbol lookup walks the list from front to back.

Closures capture the environment at creation time — they get a snapshot, not a reference. Redefining a symbol later does not affect existing closures.

## Builtins and effects

The stepper does not hardcode any specific builtins. It resolves symbols from the environment:

- **`VAL_BUILTIN`** — pure C function. The stepper extracts the function pointer and calls it with the evaluated arguments.
- **`VAL_EFFECT`** — side-effecting operation. The stepper constructs an effect request map (`{:effect-name :keyword :effect-args (args...)}`) and returns a `:suspended` state. The runtime handles the effect and resumes.

## Error handling

`step()` returns `VAL_ERROR` for:
- Malformed state (missing keys, wrong types)
- Stepping a `:done` or `:suspended` state
- Undefined symbol
- Wrong argument count for function call
- Structural errors (`if` without 3 args, odd `let` bindings, etc.)
- Errors from builtin functions (forwarded)

## Dependencies

- `val` — all state is represented as Vals
