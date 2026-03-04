# enkor

A small Lisp-like language with Clojure-inspired immutability, implemented in plain C.

C was chosen not for performance but for its simplicity — the language is small, easy to reason about, and has a straightforward build and linking process.

## Core Design

### Values

All data in enkor is represented by a single type: `Val`. It is an opaque, immutable, heap-allocated, reference-counted tagged union supporting 9 types:

- **NIL** — the absence of a value
- **BOOL** — true or false
- **INT** — 64-bit signed integer
- **FLOAT** — 64-bit double
- **STRING** — opaque UTF-8 byte sequence (not indexable, no concept of "character")
- **SYMBOL** — an identifier (e.g. `foo`, `my-fn?`), following Clojure naming rules
- **KEYWORD** — a keyword identifier (e.g. `:name`, `:type`), following Clojure naming rules
- **LIST** — an ordered sequence of values
- **MAP** — a hash map from any value to any value

### Immutability

Values are immutable once created. This is the foundational design choice:

- Reference counting is the sole memory management strategy. Because immutable values can only reference values that already exist, cycles cannot form, making refcounting complete and correct.
- Hashes are precomputed at creation time and stored on every value. This makes map operations efficient and the hash/equality contract trivial.
- Structural equality is well-defined and recursive with no ambiguity.

### Total Ordering

All values are totally ordered via `val_cmp`, which returns negative/zero/positive:

- **Cross-type:** strict type ordering by enum value — `NIL < BOOL < INT < FLOAT < STRING < SYMBOL < KEYWORD < LIST < MAP`. Values of different types are never equal (no numeric promotion between INT and FLOAT).
- **Within-type:** natural ordering — numeric for numbers, byte-level lexicographic for strings/symbols/keywords, element-wise for lists, sorted-entry-wise for maps.
- **Special cases:** `NaN == NaN` and `NaN` sorts last among floats. `-0.0 == +0.0`.

### Map Implementation

Maps are open-addressing hash tables with linear probing, built once at creation and never mutated. No deletion, no tombstones, no incremental resizing. Duplicate keys at construction are resolved as last-write-wins. Any value can be a map key.

### Strings

Strings are opaque UTF-8 byte sequences with explicitly tracked length. They may contain embedded null bytes. There is no concept of indexing into a string or of individual characters — this avoids the complexity of bytes vs codepoints vs grapheme clusters.

## Project Structure

The codebase is organized around **concepts** — each concept is a directory containing:

| File | Purpose |
|------|---------|
| `<name>.h` | Public header (opaque types, function declarations) |
| `<name>.c` | Implementation |
| `<name>.md` | Programming guide (written for LLM and human consumption) |
| `desc.txt` | One-line description of the concept |
| `deps.txt` | Dependencies, one concept name per line (empty if none) |
| `test_*.c` | Test files, one test per file |

### Current Concepts

- **val** — the core value type and its 19-function API

## Build System

The build system is a single C program (`build.c`) with no external dependencies. Bootstrap it once:

```
cc build.c -o build
```

### Running Tests

```
./build test val
```

This will:
1. Glob all `test_*.c` files in the concept directory
2. Generate a `test_main.c` test runner
3. Compile the concept source and all tests into a single executable
4. Run the tests

### Test Output

Each test runs in a forked child process so that crashes and aborts don't prevent other tests from running:

```
Found 30 tests in val/

Compiling...

  PASS  as_wrong_type_aborts
  PASS  bool
  PASS  cmp_bool
  ...
  PASS  symbol

30 passed, 0 failed, 30 total
```

## Testing Conventions

- Each test file `test_<name>.c` defines one function `void test_<name>(void)`.
- Tests use assertion macros from `test.h` that print descriptive error messages (expected vs actual values, type names, comparison results) before aborting.
- Test files ending in `_aborts` are expected to abort — the runner treats abort as PASS and clean exit as FAIL.
- See [TESTING.md](TESTING.md) for full details on assertion macros and the test runner.
