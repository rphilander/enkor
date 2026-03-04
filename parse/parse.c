#include "parse.h"
#include "tok.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* ── helpers ─────────────────────────────────────────────────────── */

typedef struct {
    Val  *tokens;   /* the VAL_LIST of token maps */
    size_t pos;     /* current position in token list */
    size_t len;     /* total number of tokens */
} Parser;

static Val *tok_type_kw;    /* cached :type keyword */
static Val *tok_value_kw;   /* cached :value keyword */
static Val *tok_line_kw;    /* cached :line keyword */
static Val *tok_col_kw;     /* cached :col keyword */

static void init_keywords(void) {
    if (!tok_type_kw) {
        tok_type_kw  = val_keyword("type");
        tok_value_kw = val_keyword("value");
        tok_line_kw  = val_keyword("line");
        tok_col_kw   = val_keyword("col");
    }
}

static Val *cur_token(Parser *p) {
    return val_list_get(p->tokens, p->pos);
}

static const char *token_type_name(Val *token) {
    Val *ty = val_map_get(token, tok_type_kw);
    return val_as_keyword(ty);
}

static Val *token_value(Val *token) {
    return val_map_get(token, tok_value_kw);
}

static Val *make_error_at(Val *token, const char *msg) {
    int line = (int)val_as_int(val_map_get(token, tok_line_kw));
    int col  = (int)val_as_int(val_map_get(token, tok_col_kw));
    char buf[256];
    snprintf(buf, sizeof(buf), "%d:%d: %s", line, col, msg);
    return val_error(buf);
}

static int at_end(Parser *p) {
    return p->pos >= p->len;
}

/* ── recursive descent ───────────────────────────────────────────── */

static Val *parse_form(Parser *p);

static Val *parse_list(Parser *p, Val *open_token) {
    /* lparen already consumed */
    size_t cap = 16;
    Val **items = malloc(cap * sizeof(Val *));
    size_t count = 0;

    while (1) {
        if (at_end(p)) {
            for (size_t i = 0; i < count; i++) val_release(items[i]);
            free(items);
            return make_error_at(open_token, "unmatched '('");
        }

        Val *token = cur_token(p);
        const char *ty = token_type_name(token);

        if (strcmp(ty, "rparen") == 0) {
            p->pos++; /* consume ) */
            break;
        }

        if (strcmp(ty, "rbrace") == 0) {
            for (size_t i = 0; i < count; i++) val_release(items[i]);
            free(items);
            return make_error_at(token, "unexpected '}' inside list");
        }

        Val *form = parse_form(p);
        if (val_type(form) == VAL_ERROR) {
            for (size_t i = 0; i < count; i++) val_release(items[i]);
            free(items);
            return form;
        }

        if (count >= cap) { cap *= 2; items = realloc(items, cap * sizeof(Val *)); }
        items[count++] = form;
    }

    Val *result = val_list(items, count);
    for (size_t i = 0; i < count; i++) val_release(items[i]);
    free(items);
    return result;
}

static Val *parse_map(Parser *p, Val *open_token) {
    /* lbrace already consumed */
    size_t cap = 16;
    Val **keys = malloc(cap * sizeof(Val *));
    Val **vals = malloc(cap * sizeof(Val *));
    size_t count = 0;

    while (1) {
        if (at_end(p)) {
            for (size_t i = 0; i < count; i++) { val_release(keys[i]); val_release(vals[i]); }
            free(keys); free(vals);
            return make_error_at(open_token, "unmatched '{'");
        }

        Val *token = cur_token(p);
        const char *ty = token_type_name(token);

        if (strcmp(ty, "rbrace") == 0) {
            p->pos++; /* consume } */
            break;
        }

        if (strcmp(ty, "rparen") == 0) {
            for (size_t i = 0; i < count; i++) { val_release(keys[i]); val_release(vals[i]); }
            free(keys); free(vals);
            return make_error_at(token, "unexpected ')' inside map");
        }

        /* parse key */
        Val *key = parse_form(p);
        if (val_type(key) == VAL_ERROR) {
            for (size_t i = 0; i < count; i++) { val_release(keys[i]); val_release(vals[i]); }
            free(keys); free(vals);
            return key;
        }

        /* parse value */
        if (at_end(p)) {
            val_release(key);
            for (size_t i = 0; i < count; i++) { val_release(keys[i]); val_release(vals[i]); }
            free(keys); free(vals);
            return make_error_at(open_token, "odd number of forms in map literal");
        }

        Val *peek = cur_token(p);
        const char *peek_ty = token_type_name(peek);
        if (strcmp(peek_ty, "rbrace") == 0) {
            val_release(key);
            for (size_t i = 0; i < count; i++) { val_release(keys[i]); val_release(vals[i]); }
            free(keys); free(vals);
            return make_error_at(open_token, "odd number of forms in map literal");
        }

        Val *value = parse_form(p);
        if (val_type(value) == VAL_ERROR) {
            val_release(key);
            for (size_t i = 0; i < count; i++) { val_release(keys[i]); val_release(vals[i]); }
            free(keys); free(vals);
            return value;
        }

        if (count >= cap) {
            cap *= 2;
            keys = realloc(keys, cap * sizeof(Val *));
            vals = realloc(vals, cap * sizeof(Val *));
        }
        keys[count] = key;
        vals[count] = value;
        count++;
    }

    Val *result = val_map(keys, vals, count);
    for (size_t i = 0; i < count; i++) { val_release(keys[i]); val_release(vals[i]); }
    free(keys); free(vals);
    return result;
}

static Val *parse_form(Parser *p) {
    Val *token = cur_token(p);
    const char *ty = token_type_name(token);
    p->pos++;

    if (strcmp(ty, "lparen") == 0) return parse_list(p, token);
    if (strcmp(ty, "lbrace") == 0) return parse_map(p, token);
    if (strcmp(ty, "rparen") == 0) return make_error_at(token, "unexpected ')'");
    if (strcmp(ty, "rbrace") == 0) return make_error_at(token, "unexpected '}'");

    /* atom: return the token's :value directly */
    Val *v = token_value(token);
    return val_retain(v);
}

/* ── public API ──────────────────────────────────────────────────── */

Val *parse(Val *input) {
    assert(val_type(input) == VAL_STRING);

    init_keywords();

    /* tokenize */
    Val *tokens = tok(input);
    if (val_type(tokens) == VAL_ERROR) return tokens;

    /* parse all top-level forms */
    Parser parser = { .tokens = tokens, .pos = 0, .len = val_len(tokens) };
    Parser *p = &parser;

    size_t cap = 16;
    Val **forms = malloc(cap * sizeof(Val *));
    size_t count = 0;

    while (!at_end(p)) {
        Val *form = parse_form(p);
        if (val_type(form) == VAL_ERROR) {
            for (size_t i = 0; i < count; i++) val_release(forms[i]);
            free(forms);
            val_release(tokens);
            return form;
        }

        if (count >= cap) { cap *= 2; forms = realloc(forms, cap * sizeof(Val *)); }
        forms[count++] = form;
    }

    Val *result = val_list(forms, count);
    for (size_t i = 0; i < count; i++) val_release(forms[i]);
    free(forms);
    val_release(tokens);
    return result;
}
