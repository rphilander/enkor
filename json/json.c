#include "json.h"
#include "vendor/cJSON.c"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ── JSON → Val ──────────────────────────────────────────────────── */

static Val *cjson_to_val(const cJSON *item) {
    if (cJSON_IsNull(item))   return val_nil();
    if (cJSON_IsTrue(item))   return val_bool(true);
    if (cJSON_IsFalse(item))  return val_bool(false);

    if (cJSON_IsNumber(item)) {
        double d = item->valuedouble;
        /* if it's a whole number that fits in int64, use VAL_INT */
        if (d == floor(d) && d >= -9007199254740992.0 && d <= 9007199254740992.0) {
            return val_int((int64_t)d);
        }
        return val_float(d);
    }

    if (cJSON_IsString(item)) {
        return val_string(item->valuestring, strlen(item->valuestring));
    }

    if (cJSON_IsArray(item)) {
        int n = cJSON_GetArraySize(item);
        Val **items = malloc(n * sizeof(Val *));
        for (int i = 0; i < n; i++) {
            items[i] = cjson_to_val(cJSON_GetArrayItem(item, i));
            if (val_type(items[i]) == VAL_ERROR) {
                Val *err = val_retain(items[i]);
                for (int j = 0; j <= i; j++) val_release(items[j]);
                free(items);
                return err;
            }
        }
        Val *list = val_list(items, n);
        for (int i = 0; i < n; i++) val_release(items[i]);
        free(items);
        return list;
    }

    if (cJSON_IsObject(item)) {
        int n = cJSON_GetArraySize(item);
        Val **keys = malloc(n * sizeof(Val *));
        Val **vals = malloc(n * sizeof(Val *));
        cJSON *child = item->child;
        for (int i = 0; i < n; i++) {
            keys[i] = val_string(child->string, strlen(child->string));
            vals[i] = cjson_to_val(child);
            if (val_type(vals[i]) == VAL_ERROR) {
                Val *err = val_retain(vals[i]);
                for (int j = 0; j <= i; j++) { val_release(keys[j]); val_release(vals[j]); }
                free(keys); free(vals);
                return err;
            }
            child = child->next;
        }
        Val *map = val_map(keys, vals, n);
        for (int i = 0; i < n; i++) { val_release(keys[i]); val_release(vals[i]); }
        free(keys); free(vals);
        return map;
    }

    return val_error("json_to_val: unsupported JSON type");
}

Val *json_to_val(Val *json_str) {
    if (val_type(json_str) != VAL_STRING) {
        return val_error("json_to_val: input must be a string");
    }
    size_t len;
    const char *str = val_as_string(json_str, &len);
    cJSON *parsed = cJSON_ParseWithLength(str, len);
    if (parsed == NULL) {
        return val_error("json_to_val: invalid JSON");
    }
    Val *result = cjson_to_val(parsed);
    cJSON_Delete(parsed);
    return result;
}

/* ── Val → JSON ──────────────────────────────────────────────────── */

static cJSON *val_to_cjson(Val *v) {
    switch (val_type(v)) {
    case VAL_NIL:
        return cJSON_CreateNull();
    case VAL_BOOL:
        return cJSON_CreateBool(val_as_bool(v));
    case VAL_INT:
        return cJSON_CreateNumber((double)val_as_int(v));
    case VAL_FLOAT:
        return cJSON_CreateNumber(val_as_float(v));
    case VAL_STRING: {
        size_t len;
        const char *s = val_as_string(v, &len);
        return cJSON_CreateString(s);
    }
    case VAL_LIST: {
        cJSON *arr = cJSON_CreateArray();
        size_t n = val_len(v);
        for (size_t i = 0; i < n; i++) {
            cJSON *child = val_to_cjson(val_list_get(v, i));
            if (child == NULL) { cJSON_Delete(arr); return NULL; }
            cJSON_AddItemToArray(arr, child);
        }
        return arr;
    }
    case VAL_MAP: {
        cJSON *obj = cJSON_CreateObject();
        size_t n = val_len(v);
        for (size_t i = 0; i < n; i++) {
            Val *key, *val;
            val_map_entry(v, i, &key, &val);
            if (val_type(key) != VAL_STRING) {
                cJSON_Delete(obj);
                return NULL;
            }
            size_t klen;
            const char *kstr = val_as_string(key, &klen);
            cJSON *child = val_to_cjson(val);
            if (child == NULL) { cJSON_Delete(obj); return NULL; }
            cJSON_AddItemToObject(obj, kstr, child);
        }
        return obj;
    }
    default:
        return NULL;
    }
}

Val *val_to_json(Val *v) {
    ValType t = val_type(v);
    if (t == VAL_SYMBOL || t == VAL_KEYWORD || t == VAL_BUILTIN ||
        t == VAL_EFFECT || t == VAL_ERROR) {
        return val_error("val_to_json: type not representable in JSON");
    }

    cJSON *json = val_to_cjson(v);
    if (json == NULL) {
        if (t == VAL_MAP) return val_error("val_to_json: map keys must be strings");
        return val_error("val_to_json: conversion failed");
    }

    char *str = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);
    Val *result = val_string(str, strlen(str));
    free(str);
    return result;
}
