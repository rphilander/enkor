#ifndef JSON_H
#define JSON_H

#include "val.h"

/*
 * Convert a JSON string to a Val.
 *
 * Input:  a VAL_STRING containing JSON text.
 * Output: the corresponding Val, or a VAL_ERROR on parse failure.
 *
 * Mapping:
 *   null    → VAL_NIL
 *   true    → VAL_BOOL (true)
 *   false   → VAL_BOOL (false)
 *   integer → VAL_INT
 *   float   → VAL_FLOAT
 *   string  → VAL_STRING
 *   array   → VAL_LIST
 *   object  → VAL_MAP (keys are VAL_STRING)
 */
Val *json_to_val(Val *json_str);

/*
 * Convert a Val to a JSON string.
 *
 * Input:  a Val (must be nil, bool, int, float, string, list, or map).
 * Output: a VAL_STRING containing JSON text, or VAL_ERROR if the Val
 *         contains types not representable in JSON (symbol, keyword,
 *         builtin, effect, error).
 *
 * Map keys must be VAL_STRING.
 */
Val *val_to_json(Val *v);

#endif
