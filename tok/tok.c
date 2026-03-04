#include "tok.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

/* ── helpers ─────────────────────────────────────────────────────── */

typedef struct {
    const char *src;
    size_t      len;
    size_t      pos;
    int         line;
    int         col;
} Lexer;

static void lexer_init(Lexer *l, const char *src, size_t len) {
    l->src  = src;
    l->len  = len;
    l->pos  = 0;
    l->line = 1;
    l->col  = 1;
}

static int at_end(const Lexer *l) {
    return l->pos >= l->len;
}

static char peek(const Lexer *l) {
    if (at_end(l)) return '\0';
    return l->src[l->pos];
}

static char advance(Lexer *l) {
    char c = l->src[l->pos++];
    if (c == '\n') { l->line++; l->col = 1; }
    else           { l->col++; }
    return c;
}

static Val *make_error(int line, int col, const char *fmt, ...) {
    char buf[256];
    int n = snprintf(buf, sizeof(buf), "%d:%d: ", line, col);
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf + n, sizeof(buf) - n, fmt, ap);
    va_end(ap);
    return val_error(buf);
}

static Val *make_token(int line, int col, const char *type, Val *value) {
    Val *keys[4], *vals[4];
    keys[0] = val_keyword("type");   vals[0] = val_keyword(type);
    keys[1] = val_keyword("line");   vals[1] = val_int(line);
    keys[2] = val_keyword("col");    vals[2] = val_int(col);
    keys[3] = val_keyword("value");  vals[3] = value;
    Val *map = val_map(keys, vals, 4);
    for (int i = 0; i < 4; i++) { val_release(keys[i]); val_release(vals[i]); }
    return map;
}

/* ── skip whitespace and comments ────────────────────────────────── */

static void skip_ws(Lexer *l) {
    while (!at_end(l)) {
        char c = peek(l);
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            advance(l);
        } else if (c == ';') {
            /* comment: skip to end of line */
            while (!at_end(l) && peek(l) != '\n') advance(l);
        } else {
            break;
        }
    }
}

/* ── symbol character predicate (Clojure-style) ──────────────────── */

static int is_symbol_start(char c) {
    if (isalpha((unsigned char)c)) return 1;
    /* Clojure allows these as symbol characters */
    return c == '+' || c == '-' || c == '*' || c == '/'
        || c == '!' || c == '?' || c == '_' || c == '='
        || c == '<' || c == '>' || c == '&' || c == '%'
        || c == '.' || c == '#';
}

static int is_symbol_cont(char c) {
    return is_symbol_start(c) || isdigit((unsigned char)c)
        || c == '\'' || c == ':';
}

/* ── tokenize a string literal ───────────────────────────────────── */

static Val *tok_string(Lexer *l, int start_line, int start_col) {
    /* opening quote already consumed */
    size_t cap = 64;
    char *buf = malloc(cap);
    size_t len = 0;

    while (!at_end(l)) {
        char c = advance(l);
        if (c == '"') {
            Val *s = val_string(buf, len);
            free(buf);
            return make_token(start_line, start_col, "string", s);
        }
        if (c == '\\') {
            if (at_end(l)) {
                free(buf);
                return make_error(start_line, start_col, "unterminated string escape");
            }
            char e = advance(l);
            switch (e) {
            case '"':  c = '"';  break;
            case '\\': c = '\\'; break;
            case 'n':  c = '\n'; break;
            case 't':  c = '\t'; break;
            default:
                free(buf);
                return make_error(l->line, l->col - 1,
                    "invalid escape sequence: \\%c", e);
            }
        }
        if (len + 1 > cap) { cap *= 2; buf = realloc(buf, cap); }
        buf[len++] = c;
    }

    free(buf);
    return make_error(start_line, start_col, "unterminated string");
}

/* ── tokenize a number (int or float) ────────────────────────────── */

static Val *tok_number(Lexer *l, int start_line, int start_col, int negative) {
    size_t begin = l->pos;

    /* consume digits before dot */
    while (!at_end(l) && isdigit((unsigned char)peek(l))) advance(l);

    int is_float = 0;
    if (!at_end(l) && peek(l) == '.') {
        /* check next char: must be digit for float */
        if (l->pos + 1 < l->len && isdigit((unsigned char)l->src[l->pos + 1])) {
            is_float = 1;
            advance(l); /* consume '.' */
            while (!at_end(l) && isdigit((unsigned char)peek(l))) advance(l);
        }
        /* else: trailing dot, treat digits as int and leave '.' */
    }

    size_t end = l->pos;
    size_t num_len = end - begin;

    /* copy the digit string for parsing */
    char tmp[64];
    if (num_len >= sizeof(tmp)) num_len = sizeof(tmp) - 1;
    memcpy(tmp, l->src + begin, num_len);
    tmp[num_len] = '\0';

    if (is_float) {
        double d = strtod(tmp, NULL);
        if (negative) d = -d;
        return make_token(start_line, start_col, "float", val_float(d));
    } else {
        int64_t n = strtoll(tmp, NULL, 10);
        if (negative) n = -n;
        return make_token(start_line, start_col, "int", val_int(n));
    }
}

/* ── tokenize a symbol or keyword ────────────────────────────────── */

static Val *tok_symbol(Lexer *l, int start_line, int start_col) {
    size_t begin = l->pos - 1; /* we already consumed first char */

    while (!at_end(l) && is_symbol_cont(peek(l))) advance(l);

    size_t len = l->pos - begin;
    char tmp[256];
    if (len >= sizeof(tmp)) len = sizeof(tmp) - 1;
    memcpy(tmp, l->src + begin, len);
    tmp[len] = '\0';

    /* check for nil / true / false */
    if (strcmp(tmp, "nil") == 0)
        return make_token(start_line, start_col, "nil", val_nil());
    if (strcmp(tmp, "true") == 0)
        return make_token(start_line, start_col, "true", val_bool(true));
    if (strcmp(tmp, "false") == 0)
        return make_token(start_line, start_col, "false", val_bool(false));

    return make_token(start_line, start_col, "symbol", val_symbol(tmp));
}

static Val *tok_keyword(Lexer *l, int start_line, int start_col) {
    /* ':' already consumed */
    size_t begin = l->pos;

    if (at_end(l) || !isalpha((unsigned char)peek(l))) {
        return make_error(start_line, start_col, "expected keyword name after ':'");
    }

    while (!at_end(l) && is_symbol_cont(peek(l))) advance(l);

    size_t len = l->pos - begin;
    char tmp[256];
    if (len >= sizeof(tmp)) len = sizeof(tmp) - 1;
    memcpy(tmp, l->src + begin, len);
    tmp[len] = '\0';

    return make_token(start_line, start_col, "keyword", val_keyword(tmp));
}

/* ── main tokenizer ──────────────────────────────────────────────── */

Val *tok(Val *input) {
    assert(val_type(input) == VAL_STRING);

    size_t src_len;
    const char *src = val_as_string(input, &src_len);

    Lexer lexer;
    lexer_init(&lexer, src, src_len);
    Lexer *l = &lexer;

    /* dynamic array of tokens */
    size_t cap = 64;
    Val **tokens = malloc(cap * sizeof(Val *));
    size_t count = 0;

    while (1) {
        skip_ws(l);
        if (at_end(l)) break;

        int start_line = l->line;
        int start_col  = l->col;
        char c = advance(l);

        Val *token = NULL;

        switch (c) {
        case '(':
            token = make_token(start_line, start_col, "lparen", val_nil());
            break;
        case ')':
            token = make_token(start_line, start_col, "rparen", val_nil());
            break;
        case '{':
            token = make_token(start_line, start_col, "lbrace", val_nil());
            break;
        case '}':
            token = make_token(start_line, start_col, "rbrace", val_nil());
            break;
        case '[':
        case ']':
            token = make_error(start_line, start_col,
                "brackets are not supported in enkor — use () for lists");
            break;
        case ',':
            token = make_error(start_line, start_col,
                "commas are not valid syntax in enkor");
            break;
        case '"':
            token = tok_string(l, start_line, start_col);
            break;
        case ':':
            token = tok_keyword(l, start_line, start_col);
            break;
        case '-':
            if (!at_end(l) && isdigit((unsigned char)peek(l))) {
                token = tok_number(l, start_line, start_col, 1);
            } else {
                token = tok_symbol(l, start_line, start_col);
            }
            break;
        default:
            if (isdigit((unsigned char)c)) {
                /* back up: tok_number expects pos at first digit */
                l->pos--;
                l->col--;
                token = tok_number(l, start_line, start_col, 0);
            } else if (is_symbol_start(c)) {
                token = tok_symbol(l, start_line, start_col);
            } else {
                token = make_error(start_line, start_col,
                    "unexpected character: '%c'", c);
            }
            break;
        }

        /* If we got an error, clean up and return it */
        if (val_type(token) == VAL_ERROR) {
            for (size_t i = 0; i < count; i++) val_release(tokens[i]);
            free(tokens);
            return token;
        }

        if (count >= cap) { cap *= 2; tokens = realloc(tokens, cap * sizeof(Val *)); }
        tokens[count++] = token;
    }

    Val *result = val_list(tokens, count);
    for (size_t i = 0; i < count; i++) val_release(tokens[i]);
    free(tokens);
    return result;
}
