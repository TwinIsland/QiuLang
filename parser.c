#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "parser.h"
#include "lexer.h"
#include "ir.h"

static Token *current_token;
static int label_count = 0;

void eat(TokenType type)
{
    if (current_token->type == type)
    {
        current_token = token_next();
    }
    else
    {
        SET_ERR("Syntax error: unexpected token");
        exit(EXIT_FAILURE);
    }
}

// Parsing Rule Start
RULE(expression);
RULE(term);
RULE(factor);
RULE(statement);
// RULE(block);

RULE(expression)
{
    term();
    while (current_token->type == TOK_ADD || current_token->type == TOK_MIN)
    {
        if (current_token->type == TOK_ADD)
        {
            eat(TOK_ADD);
            term();
            ir_emit(IR_ADD, 0, NULL, 0);
        }
        else if (current_token->type == TOK_MIN)
        {
            eat(TOK_MIN);
            term();
            ir_emit(IR_SUB, 0, NULL, 0);
        }
    }
}

RULE(term)
{
    factor();
    while (current_token->type == TOK_MULT || current_token->type == TOK_DIV)
    {
        if (current_token->type == TOK_MULT)
        {
            eat(TOK_MULT);
            factor();
            ir_emit(IR_MUL, 0, NULL, 0);
        }
        else if (current_token->type == TOK_DIV)
        {
            eat(TOK_DIV);
            factor();
            ir_emit(IR_DIV, 0, NULL, 0);
        }
    }
}

RULE(factor)
{
    if (current_token->type == TOK_INT || current_token->type == TOK_FLOAT)
    {
        ir_emit(IR_PUSH, atof(current_token->value), NULL, 0);
        eat(current_token->type);
    }
    else if (current_token->type == TOK_LPAREN)
    {
        eat(TOK_LPAREN);
        expression();
        eat(TOK_RPAREN);
    }
    else
    {
        SET_ERR("Syntax error: unexpected token in factor");
        exit(EXIT_FAILURE);
    }
}

RULE(statement)
{
    if (current_token->type == TOK_LET)
    {
        eat(TOK_LET);
        if (current_token->type == TOK_IDENT)
        {
            char ident[32];

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
            strncpy(ident, (char *)current_token->value, sizeof(ident));
#pragma GCC diagnostic pop

            eat(TOK_IDENT);
            eat(TOK_ASSIGN);
            expression();
            ir_emit(IR_ASSIGN, 0, ident, 0);
            eat(TOK_SEMI);
        }
        else
        {
            SET_ERR("Syntax error: expected identifier after 'let'");
            exit(EXIT_FAILURE);
        }
    }
    else if (current_token->type == TOK_IF)
    {
        eat(TOK_IF);
        eat(TOK_LPAREN);
        expression();
        eat(TOK_RPAREN);
        int false_label = label_count++;
        ir_emit(IR_JMP_IF_FALSE, 0, NULL, false_label);
        statement();
        if (current_token->type == TOK_ELSE)
        {
            eat(TOK_ELSE);
            int end_label = label_count++;
            ir_emit(IR_NOP, 0, NULL, end_label);
            ir_emit(IR_LABEL, 0, NULL, false_label);
            statement();
            ir_emit(IR_LABEL, 0, NULL, end_label);
        }
        else
        {
            ir_emit(IR_LABEL, 0, NULL, false_label);
        }
    }
    else if (current_token->type == TOK_PRINT)
    {
        eat(TOK_PRINT);
        if (current_token->type == TOK_STRING)
        {
            ir_emit(IR_PRINT, 0, (char *)current_token->value, 0);
            eat(TOK_STRING);
            eat(TOK_SEMI);
        }
        else
        {
            SET_ERR("Syntax error: expected string after 'print'");
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        SET_ERR("Syntax error: unexpected token in statement");
        exit(EXIT_FAILURE);
    }
}

// RULE(block)
// {
//     eat(TOK_LCBRANCE);
//     while (current_token->type != TOK_RCBRANCE)
//     {
//         statement();
//     }
//     eat(TOK_RCBRANCE);
// }
// Parsing Rule End

void parse(const char *expression)
{
    current_token = token_next();
    while (current_token != NULL)
    {
        if (is_err)
        {
            printf("Parsing failed: %s", err_msg);
        }
        statement();
    }
}

#ifdef PARSING
int main()
{
    const char *expression = "let a = 4.1; if (a / 2 = 3) { print(\"asdasdadss\"); } else if (true) { print(\"wtf\"); };";
    parse(expression);

    printf("Intermediate Representation:\n");
    ir_print();

    printf("\nGenerated Assembly:\n");
    ir_generate_asm();

    return 0;
}
#endif