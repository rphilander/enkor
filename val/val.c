#include "val.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

struct Val {
    ValType type;
    int refcount;
    uint64_t hash_val;
    union {
        bool boolean;
        int64_t integer;
        double floating;
        struct { char *data; size_t len; } string;
        struct { Val **items; size_t len; } list;
        struct { Val **keys; Val **vals; size_t cap; size_t len; } map;
        struct { char *name; BuiltinFn fn; } builtin;
        struct { char *effect_name; } effect;
    } as;
};

/* Forward declarations */
int val_cmp(const Val *a, const Val *b);

/* --- Hashing (FNV-1a + boost-style combine) --- */

#define FNV_OFFSET 14695981039346656037ULL
#define FNV_PRIME  1099511628211ULL

static uint64_t hash_bytes(const void *data, size_t len) {
    uint64_t h = FNV_OFFSET;
    const uint8_t *p = data;
    for (size_t i = 0; i < len; i++) {
        h ^= p[i];
        h *= FNV_PRIME;
    }
    return h;
}

static uint64_t hash_combine(uint64_t a, uint64_t b) {
    a ^= b + 0x9e3779b97f4a7c15ULL + (a << 6) + (a >> 2);
    return a;
}

static uint64_t compute_hash(const Val *v) {
    uint64_t h = hash_bytes(&v->type, sizeof(v->type));

    switch (v->type) {
    case VAL_NIL:
        break;
    case VAL_BOOL:
        h = hash_combine(h, v->as.boolean ? 1ULL : 0ULL);
        break;
    case VAL_INT: {
        uint64_t bits;
        memcpy(&bits, &v->as.integer, sizeof(bits));
        h = hash_combine(h, bits);
        break;
    }
    case VAL_FLOAT: {
        double d = v->as.floating;
        if (isnan(d)) {
            h = hash_combine(h, 0x7FF8000000000001ULL);
        } else {
            if (d == 0.0) d = 0.0; /* normalize -0.0 */
            uint64_t bits;
            memcpy(&bits, &d, sizeof(bits));
            h = hash_combine(h, bits);
        }
        break;
    }
    case VAL_STRING:
    case VAL_SYMBOL:
    case VAL_KEYWORD:
    case VAL_ERROR: {
        uint64_t sh = hash_bytes(v->as.string.data, v->as.string.len);
        h = hash_combine(h, sh);
        break;
    }
    case VAL_BUILTIN: {
        size_t nlen = strlen(v->as.builtin.name);
        uint64_t nh = hash_bytes(v->as.builtin.name, nlen);
        h = hash_combine(h, nh);
        break;
    }
    case VAL_EFFECT: {
        size_t nlen = strlen(v->as.effect.effect_name);
        uint64_t nh = hash_bytes(v->as.effect.effect_name, nlen);
        h = hash_combine(h, nh);
        break;
    }
    case VAL_LIST:
        for (size_t i = 0; i < v->as.list.len; i++) {
            h = hash_combine(h, v->as.list.items[i]->hash_val);
        }
        break;
    case VAL_MAP: {
        /* Order-independent: XOR all pair hashes */
        uint64_t mh = 0;
        for (size_t i = 0; i < v->as.map.cap; i++) {
            if (v->as.map.keys[i] != NULL) {
                uint64_t ph = hash_combine(v->as.map.keys[i]->hash_val,
                                           v->as.map.vals[i]->hash_val);
                mh ^= ph;
            }
        }
        h = hash_combine(h, mh);
        break;
    }
    }

    return h;
}

/* --- Allocation --- */

static Val *val_alloc(ValType type) {
    Val *v = calloc(1, sizeof(Val));
    assert(v != NULL);
    v->type = type;
    v->refcount = 1;
    return v;
}

/* --- Constructors --- */

Val *val_nil(void) {
    Val *v = val_alloc(VAL_NIL);
    v->hash_val = compute_hash(v);
    return v;
}

Val *val_bool(bool b) {
    Val *v = val_alloc(VAL_BOOL);
    v->as.boolean = b;
    v->hash_val = compute_hash(v);
    return v;
}

Val *val_int(int64_t n) {
    Val *v = val_alloc(VAL_INT);
    v->as.integer = n;
    v->hash_val = compute_hash(v);
    return v;
}

Val *val_float(double d) {
    Val *v = val_alloc(VAL_FLOAT);
    v->as.floating = d;
    v->hash_val = compute_hash(v);
    return v;
}

Val *val_string(const char *data, size_t len) {
    Val *v = val_alloc(VAL_STRING);
    v->as.string.data = malloc(len + 1);
    assert(v->as.string.data != NULL);
    memcpy(v->as.string.data, data, len);
    v->as.string.data[len] = '\0';
    v->as.string.len = len;
    v->hash_val = compute_hash(v);
    return v;
}

Val *val_symbol(const char *name) {
    Val *v = val_alloc(VAL_SYMBOL);
    size_t len = strlen(name);
    v->as.string.data = malloc(len + 1);
    assert(v->as.string.data != NULL);
    memcpy(v->as.string.data, name, len + 1);
    v->as.string.len = len;
    v->hash_val = compute_hash(v);
    return v;
}

Val *val_keyword(const char *name) {
    Val *v = val_alloc(VAL_KEYWORD);
    size_t len = strlen(name);
    v->as.string.data = malloc(len + 1);
    assert(v->as.string.data != NULL);
    memcpy(v->as.string.data, name, len + 1);
    v->as.string.len = len;
    v->hash_val = compute_hash(v);
    return v;
}

Val *val_error(const char *message) {
    Val *v = val_alloc(VAL_ERROR);
    size_t len = strlen(message);
    v->as.string.data = malloc(len + 1);
    assert(v->as.string.data != NULL);
    memcpy(v->as.string.data, message, len + 1);
    v->as.string.len = len;
    v->hash_val = compute_hash(v);
    return v;
}

Val *val_builtin(const char *name, BuiltinFn fn) {
    assert(fn != NULL);
    Val *v = val_alloc(VAL_BUILTIN);
    size_t len = strlen(name);
    v->as.builtin.name = malloc(len + 1);
    assert(v->as.builtin.name != NULL);
    memcpy(v->as.builtin.name, name, len + 1);
    v->as.builtin.fn = fn;
    v->hash_val = compute_hash(v);
    return v;
}

Val *val_effect(const char *name) {
    Val *v = val_alloc(VAL_EFFECT);
    size_t len = strlen(name);
    v->as.effect.effect_name = malloc(len + 1);
    assert(v->as.effect.effect_name != NULL);
    memcpy(v->as.effect.effect_name, name, len + 1);
    v->hash_val = compute_hash(v);
    return v;
}

Val *val_list(Val **items, size_t len) {
    Val *v = val_alloc(VAL_LIST);
    if (len > 0) {
        v->as.list.items = malloc(sizeof(Val *) * len);
        assert(v->as.list.items != NULL);
        for (size_t i = 0; i < len; i++) {
            v->as.list.items[i] = val_retain(items[i]);
        }
    }
    v->as.list.len = len;
    v->hash_val = compute_hash(v);
    return v;
}

Val *val_map(Val **keys, Val **vals, size_t len) {
    Val *v = val_alloc(VAL_MAP);

    if (len == 0) {
        v->hash_val = compute_hash(v);
        return v;
    }

    size_t cap = 8;
    while (cap < len * 2) cap *= 2;

    v->as.map.cap = cap;
    v->as.map.keys = calloc(cap, sizeof(Val *));
    v->as.map.vals = calloc(cap, sizeof(Val *));
    assert(v->as.map.keys != NULL);
    assert(v->as.map.vals != NULL);

    size_t count = 0;
    for (size_t i = 0; i < len; i++) {
        uint64_t slot = keys[i]->hash_val & (cap - 1);
        for (;;) {
            if (v->as.map.keys[slot] == NULL) {
                v->as.map.keys[slot] = val_retain(keys[i]);
                v->as.map.vals[slot] = val_retain(vals[i]);
                count++;
                break;
            }
            if (val_cmp(v->as.map.keys[slot], keys[i]) == 0) {
                /* Duplicate key: last write wins */
                val_release(v->as.map.vals[slot]);
                v->as.map.vals[slot] = val_retain(vals[i]);
                break;
            }
            slot = (slot + 1) & (cap - 1);
        }
    }

    v->as.map.len = count;
    v->hash_val = compute_hash(v);
    return v;
}

/* --- Memory management --- */

Val *val_retain(Val *v) {
    if (v != NULL) v->refcount++;
    return v;
}

void val_release(Val *v) {
    if (v == NULL) return;
    if (--v->refcount > 0) return;

    switch (v->type) {
    case VAL_NIL:
    case VAL_BOOL:
    case VAL_INT:
    case VAL_FLOAT:
        break;
    case VAL_STRING:
    case VAL_SYMBOL:
    case VAL_KEYWORD:
    case VAL_ERROR:
        free(v->as.string.data);
        break;
    case VAL_BUILTIN:
        free(v->as.builtin.name);
        break;
    case VAL_EFFECT:
        free(v->as.effect.effect_name);
        break;
    case VAL_LIST:
        for (size_t i = 0; i < v->as.list.len; i++) {
            val_release(v->as.list.items[i]);
        }
        free(v->as.list.items);
        break;
    case VAL_MAP:
        for (size_t i = 0; i < v->as.map.cap; i++) {
            if (v->as.map.keys[i] != NULL) {
                val_release(v->as.map.keys[i]);
                val_release(v->as.map.vals[i]);
            }
        }
        free(v->as.map.keys);
        free(v->as.map.vals);
        break;
    }
    free(v);
}

/* --- Inspection --- */

ValType val_type(const Val *v) {
    assert(v != NULL);
    return v->type;
}

uint64_t val_hash(const Val *v) {
    assert(v != NULL);
    return v->hash_val;
}

/* --- Comparison --- */

typedef struct {
    Val *key;
    Val *val;
} MapEntry;

static int entry_cmp(const void *a, const void *b) {
    const MapEntry *ea = a;
    const MapEntry *eb = b;
    int c = val_cmp(ea->key, eb->key);
    if (c != 0) return c;
    return val_cmp(ea->val, eb->val);
}

static void collect_entries(const Val *m, MapEntry *out) {
    size_t j = 0;
    for (size_t i = 0; i < m->as.map.cap; i++) {
        if (m->as.map.keys[i] != NULL) {
            out[j].key = m->as.map.keys[i];
            out[j].val = m->as.map.vals[i];
            j++;
        }
    }
}

int val_cmp(const Val *a, const Val *b) {
    assert(a != NULL && b != NULL);

    if (a->type != b->type) {
        return (a->type < b->type) ? -1 : 1;
    }

    switch (a->type) {
    case VAL_NIL:
        return 0;

    case VAL_BOOL:
        return (int)a->as.boolean - (int)b->as.boolean;

    case VAL_INT:
        if (a->as.integer < b->as.integer) return -1;
        if (a->as.integer > b->as.integer) return 1;
        return 0;

    case VAL_FLOAT: {
        bool an = isnan(a->as.floating);
        bool bn = isnan(b->as.floating);
        if (an && bn) return 0;
        if (an) return 1;
        if (bn) return -1;
        if (a->as.floating < b->as.floating) return -1;
        if (a->as.floating > b->as.floating) return 1;
        return 0;
    }

    case VAL_STRING:
    case VAL_SYMBOL:
    case VAL_KEYWORD:
    case VAL_ERROR: {
        size_t min_len = a->as.string.len < b->as.string.len
                       ? a->as.string.len : b->as.string.len;
        int c = memcmp(a->as.string.data, b->as.string.data, min_len);
        if (c != 0) return c < 0 ? -1 : 1;
        if (a->as.string.len < b->as.string.len) return -1;
        if (a->as.string.len > b->as.string.len) return 1;
        return 0;
    }

    case VAL_LIST: {
        size_t min_len = a->as.list.len < b->as.list.len
                       ? a->as.list.len : b->as.list.len;
        for (size_t i = 0; i < min_len; i++) {
            int c = val_cmp(a->as.list.items[i], b->as.list.items[i]);
            if (c != 0) return c;
        }
        if (a->as.list.len < b->as.list.len) return -1;
        if (a->as.list.len > b->as.list.len) return 1;
        return 0;
    }

    case VAL_BUILTIN: {
        int c = strcmp(a->as.builtin.name, b->as.builtin.name);
        return c < 0 ? -1 : c > 0 ? 1 : 0;
    }

    case VAL_EFFECT: {
        int c = strcmp(a->as.effect.effect_name, b->as.effect.effect_name);
        return c < 0 ? -1 : c > 0 ? 1 : 0;
    }

    case VAL_MAP: {
        if (a->as.map.len != b->as.map.len) {
            return a->as.map.len < b->as.map.len ? -1 : 1;
        }
        size_t n = a->as.map.len;
        if (n == 0) return 0;

        MapEntry *ea = malloc(sizeof(MapEntry) * n);
        MapEntry *eb = malloc(sizeof(MapEntry) * n);
        assert(ea != NULL && eb != NULL);

        collect_entries(a, ea);
        collect_entries(b, eb);

        qsort(ea, n, sizeof(MapEntry), entry_cmp);
        qsort(eb, n, sizeof(MapEntry), entry_cmp);

        int result = 0;
        for (size_t i = 0; i < n; i++) {
            result = entry_cmp(&ea[i], &eb[i]);
            if (result != 0) break;
        }

        free(ea);
        free(eb);
        return result;
    }
    }

    return 0; /* unreachable */
}

/* --- Accessors --- */

bool val_as_bool(const Val *v) {
    assert(v != NULL && v->type == VAL_BOOL);
    return v->as.boolean;
}

int64_t val_as_int(const Val *v) {
    assert(v != NULL && v->type == VAL_INT);
    return v->as.integer;
}

double val_as_float(const Val *v) {
    assert(v != NULL && v->type == VAL_FLOAT);
    return v->as.floating;
}

const char *val_as_string(const Val *v, size_t *len) {
    assert(v != NULL && v->type == VAL_STRING);
    if (len != NULL) *len = v->as.string.len;
    return v->as.string.data;
}

const char *val_as_symbol(const Val *v) {
    assert(v != NULL && v->type == VAL_SYMBOL);
    return v->as.string.data;
}

const char *val_as_keyword(const Val *v) {
    assert(v != NULL && v->type == VAL_KEYWORD);
    return v->as.string.data;
}

const char *val_as_error(const Val *v) {
    assert(v != NULL && v->type == VAL_ERROR);
    return v->as.string.data;
}

const char *val_as_builtin_name(const Val *v) {
    assert(v != NULL && v->type == VAL_BUILTIN);
    return v->as.builtin.name;
}

BuiltinFn val_as_builtin(const Val *v) {
    assert(v != NULL && v->type == VAL_BUILTIN);
    return v->as.builtin.fn;
}

const char *val_as_effect(const Val *v) {
    assert(v != NULL && v->type == VAL_EFFECT);
    return v->as.effect.effect_name;
}

/* --- Collections --- */

size_t val_len(const Val *v) {
    assert(v != NULL);
    switch (v->type) {
    case VAL_STRING:
    case VAL_SYMBOL:
    case VAL_KEYWORD:
        return v->as.string.len;
    case VAL_LIST:
        return v->as.list.len;
    case VAL_MAP:
        return v->as.map.len;
    default:
        assert(0 && "val_len: unsupported type");
        return 0;
    }
}

Val *val_list_get(const Val *v, size_t index) {
    assert(v != NULL && v->type == VAL_LIST);
    assert(index < v->as.list.len);
    return v->as.list.items[index];
}

Val *val_map_get(const Val *map, const Val *key) {
    assert(map != NULL && map->type == VAL_MAP);
    assert(key != NULL);
    if (map->as.map.len == 0) return NULL;

    uint64_t slot = key->hash_val & (map->as.map.cap - 1);
    for (;;) {
        if (map->as.map.keys[slot] == NULL) return NULL;
        if (val_cmp(map->as.map.keys[slot], key) == 0) {
            return map->as.map.vals[slot];
        }
        slot = (slot + 1) & (map->as.map.cap - 1);
    }
}
