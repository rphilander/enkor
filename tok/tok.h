#ifndef TOK_H
#define TOK_H

#include "val.h"

/*
 * Tokenize enkor source text.
 *
 * Input:  a VAL_STRING containing source text.
 * Output: a VAL_LIST of VAL_MAP tokens, or a VAL_ERROR.
 *
 * Each token map has these keyword keys:
 *   :type  — a keyword indicating the token type
 *   :line  — a VAL_INT (1-based line number)
 *   :col   — a VAL_INT (1-based column number)
 *   :value — the parsed value (type depends on token type)
 *
 * Token types and their :value types:
 *   :nil      — VAL_NIL
 *   :true     — VAL_BOOL (true)
 *   :false    — VAL_BOOL (false)
 *   :int      — VAL_INT
 *   :float    — VAL_FLOAT
 *   :string   — VAL_STRING
 *   :symbol   — VAL_SYMBOL
 *   :keyword  — VAL_KEYWORD
 *   :lparen   — VAL_NIL
 *   :rparen   — VAL_NIL
 *   :lbrace   — VAL_NIL
 *   :rbrace   — VAL_NIL
 */
Val *tok(Val *input);

#endif
