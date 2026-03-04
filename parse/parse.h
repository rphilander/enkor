#ifndef PARSE_H
#define PARSE_H

#include "val.h"

/*
 * Parse enkor source text into Val structures.
 *
 * Input:  a VAL_STRING containing enkor source text.
 * Output: a VAL_LIST of top-level forms, or a VAL_ERROR.
 *
 * Tokenizes the input via tok(), then builds nested Val structures
 * from the token stream. Atoms become their corresponding Val types.
 * (...) becomes VAL_LIST. {...} becomes VAL_MAP.
 *
 * Errors are returned for:
 *   - tokenizer errors (forwarded from tok)
 *   - unmatched ( or {
 *   - unexpected ) or }
 *   - odd number of forms in a map literal
 */
Val *parse(Val *input);

#endif
