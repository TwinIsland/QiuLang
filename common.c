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

char *token_to_str(Token *token)
{
    if (!token)
        return NULL;

    char *str = NULL;
    switch (token->type)
    {
    case TOK_IF:
        str = strdup("IF");
        break;
    case TOK_ELSE:
        str = strdup("ELSE");
        break;
    case TOK_TRUE:
        str = strdup("TRUE");
        break;
    case TOK_FALSE:
        str = strdup("FALSE");
        break;
    case TOK_LET:
        str = strdup("LET");
        break;
    case TOK_INT:
        str = (char *)malloc(5 + strlen((char *)token->value) + 1);
        sprintf(str, "INT(%s)", (char *)token->value);
        break;
    case TOK_FLOAT:
        str = (char *)malloc(7 + strlen((char *)token->value) + 1);
        sprintf(str, "FLOAT(%s)", (char *)token->value);
        break;
    case TOK_STRING:
        str = (char *)malloc(9 + strlen((char *)token->value) + 3);
        sprintf(str, "STRING(\"%s\")", (char *)token->value);
        break;
    case TOK_ADD:
        str = strdup("ADD");
        break;
    case TOK_MIN:
        str = strdup("MIN");
        break;
    case TOK_PRINT:
        str = strdup("PRINT");
        break;
    case TOK_MULT:
        str = strdup("MULT");
        break;
    case TOK_DIV:
        str = strdup("DIV");
        break;
    case TOK_IDENT:
        str = (char *)malloc(8 + strlen((char *)token->value) + 1);
        sprintf(str, "IDENT(%s)", (char *)token->value);
        break;
    case TOK_ASSIGN:
        str = strdup("ASSIGN");
        break;
    case TOK_SEMI:
        str = strdup("SEMI");
        break;
    case TOK_LPAREN:
        str = strdup("LPAREN");
        break;
    case TOK_RPAREN:
        str = strdup("RPAREN");
        break;
    case TOK_LCBRANCE:
        str = strdup("LCBRANCE");
        break;
    case TOK_RCBRANCE:
        str = strdup("RCBRANCE");
        break;
    default:
        str = strdup("UNKNOWN");
        break;
    }

    return str;
}

void print_token(Token *token)
{
    if (!token)
        return;
    char *tstr = token_to_str(token);
    puts(tstr);
    free(tstr);
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
