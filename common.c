#include "common.h"

char err_msg[128];
int is_err = 0;

Token *create_token(TokenType type, void *value)
{
    // if no value associate, set to NULL
    Token *token = (Token *)malloc(sizeof(Token));
    token->type = type;
    token->value = value;
    return token;
}

void print_token(Token *token)
{
    if (!token)
        return;
    switch (token->type)
    {
    case TOK_IF:
        printf("IF");
        break;
    case TOK_ELSE:
        printf("ELSE");
        break;
    case TOK_TRUE:
        printf("TRUE");
        break;
    case TOK_LET:
        printf("LET");
        break;
    case TOK_FALSE:
        printf("FALSE");
        break;
    case TOK_INT:
        printf("INT(%s)", (char *)token->value);
        break;
    case TOK_FLOAT:
        printf("FLOAT(%s)", (char *)token->value);
        break;
    case TOK_STRING:
        printf("STRING(\"%s\")", (char *)token->value);
        break;
    case TOK_ADD:
        printf("ADD");
        break;
    case TOK_MIN:
        printf("MIN");
        break;
    case TOK_PRINT:
        printf("PRINT");
        break;
    case TOK_MULT:
        printf("MULT");
        break;
    case TOK_DIV:
        printf("DIV");
        break;
    case TOK_IDENT:
        printf("IDENT(%s)", (char *)token->value);
        break;
    case TOK_ASSIGN:
        printf("ASSIGN");
        break;
    case TOK_SEMI:
        printf("SEMI");
        break;
    case TOK_LPAREN:
        printf("LPAREN");
        break;
    case TOK_RPAREN:
        printf("RPAREN");
        break;
    case TOK_LCBRANCE:
        printf("LCBRANCE");
        break;
    case TOK_RCBRANCE:
        printf("RCBRANCE");
        break;
    default:
        printf("UNKNOWN");
        break;
    }
    printf("\n");
}

void free_token(Token *token)
{
    free(token->value);
    free(token);
}

expr *create_var(char *name)
{
    expr *e = (expr *)malloc(sizeof(expr));
    e->type = VAR;
    e->data.var.name = name;
    return e;
}

expr *create_int(int value)
{
    expr *e = (expr *)malloc(sizeof(expr));
    e->type = INT;
    e->data.int_val.value = value;
    return e;
}

expr *create_bool(int value)
{
    expr *e = (expr *)malloc(sizeof(expr));
    e->type = BOOL;
    e->data.bool_val.value = value;
    return e;
}

expr *create_binop(bop op, expr *left, expr *right)
{
    expr *e = (expr *)malloc(sizeof(expr));
    e->type = BINOP;
    e->data.binop.op = op;
    e->data.binop.left = left;
    e->data.binop.right = right;
    return e;
}

expr *create_let(char *name, expr *value, expr *body)
{
    expr *e = (expr *)malloc(sizeof(expr));
    e->type = LET;
    e->data.let.name = strdup(name);
    e->data.let.value = value;
    e->data.let.body = body;
    return e;
}

expr *create_if(expr *cond, expr *then_branch, expr *else_branch)
{
    expr *e = (expr *)malloc(sizeof(expr));
    e->type = IF;
    e->data.if_expr.cond = cond;
    e->data.if_expr.then_branch = then_branch;
    e->data.if_expr.else_branch = else_branch;
    return e;
}

void free_expr(expr *e)
{
    if (e == NULL)
        return;
    switch (e->type)
    {
    case VAR:
        free(e->data.var.name);
        break;
    case BINOP:
        free_expr(e->data.binop.left);
        free_expr(e->data.binop.right);
        break;
    case LET:
        free(e->data.let.name);
        free_expr(e->data.let.value);
        free_expr(e->data.let.body);
        break;
    case IF:
        free_expr(e->data.if_expr.cond);
        free_expr(e->data.if_expr.then_branch);
        free_expr(e->data.if_expr.else_branch);
        break;
    default:
        break;
    }
    free(e);
}

