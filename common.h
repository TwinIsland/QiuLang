#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef DEBUG
#define debug(msg, ...)                             \
    do                                              \
    {                                               \
        printf("(DEBUG) " msg "\n", ##__VA_ARGS__); \
    } while (0)
#else
#define debug(msg, ...)
#endif

// tokens
typedef enum
{
    TOK_IF,       // if
    TOK_ELSE,     // else
    TOK_TRUE,     // true
    TOK_FALSE,    // false
    TOK_INT,      // [0 - 9]*
    TOK_STRING,   // "xxx"
    TOK_ADD,      // +
    TOK_MIN,      // -
    TOK_MULT,     // *
    TOK_DIV,      // /
    TOK_IDENT,    // xxx
    TOK_LET,      // let
    TOK_PRINT,    // print
    TOK_ASSIGN,   // =
    TOK_LPAREN,   // (
    TOK_RPAREN,   // )
    TOK_LCBRANCE, // {
    TOK_RCBRANCE, // }
    TOK_SEMI,     // ;
} TokenType;

typedef struct
{
    TokenType type;
    void *value; // optioanl
} Token;

Token *create_token(TokenType type, void *value);

void print_token(Token *token);

void free_token(Token *token);

// expression
typedef enum
{
    ADD,
    MULT,
    LEQ
} bop;

typedef struct expr expr;

typedef enum
{
    VAR,
    INT,
    BOOL,
    BINOP,
    LET,
    IF
} expr_type;

struct expr
{
    expr_type type;
    union
    {
        struct
        {
            char *name;
        } var;
        struct
        {
            int value;
        } int_val;
        struct
        {
            int value;
        } bool_val;
        struct
        {
            bop op;
            expr *left;
            expr *right;
        } binop;
        struct
        {
            char *name;
            expr *value;
            expr *body;
        } let;
        struct
        {
            expr *cond;
            expr *then_branch;
            expr *else_branch;
        } if_expr;
    } data;
};

expr *create_var(char *name);
expr *create_int(int value);
expr *create_bool(int value);
expr *create_binop(bop op, expr *left, expr *right);
expr *create_let(char *name, expr *value, expr *body);
expr *create_if(expr *cond, expr *then_branch, expr *else_branch);

void free_expr(expr *e);

// error
extern char err_msg[128];
extern int is_err;

#define SET_ERR(msg, ...)                     \
    do                                        \
    {                                         \
        sprintf(err_msg, msg, ##__VA_ARGS__); \
        is_err = 1;                           \
    } while (0);

