#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern FILE *fstream;

static inline char buf_iter()
{
    return fgetc(fstream);
}

static inline void buf_uniter(char ch)
{
    if (ch != EOF)
        ungetc(ch, fstream);
}

char buf_peek()
{
    char ret = buf_iter();
    buf_uniter(ret);
    return ret;
}

void buf_str_peek(size_t str_size, char *dest)
{
    // this function alloc buf on heap, remember to free
    int i = 0;
    for (; i < str_size; ++i)
    {
        dest[i] = buf_iter();
        if (dest[i] == EOF)
            break;
    }
    for (int j = i - 1; j >= 0; --j)
    {
        buf_uniter(dest[j]);
    }
    dest[i] = '\0';
}

void MATCH_wp()
{
    char c;
    while ((c = buf_iter()) != EOF && (c == ' ' || c == '\t' || c == '\n'))
        ;
    buf_uniter(c);
}

void MATCH_inline_comment()
{
    char c;
    while ((c = buf_iter()) != EOF && c != '\n')
        ;
}

void MATCH_multiline_comment()
{
    char *from = malloc(3);
    if (!from)
    {
        SET_ERR("Memory allocation failed");
        return;
    }

    while (buf_str_peek(2, from), strcmp(from, "*/"))
    {
        if (buf_iter() == EOF)
            break;
    }
    buf_iter(); // To move past '*'
    buf_iter(); // To move past '/'
    free(from);
}

Token *MATCH_num()
{
    char c;
    int buf_idx = 0;
    char *buf = malloc(64);
    if (!buf)
    {
        SET_ERR("Memory allocation failed");
        return NULL;
    }

    int is_float = 0;

    while ((c = buf_iter()) != EOF && (isdigit(c) || (c == '.' && (is_float = 1))))
    {
        buf[buf_idx++] = c;
        if (buf_idx == 64)
        {
            SET_ERR("Number cannot be larger than 2^64");
            free(buf);
            return NULL;
        }
    }

    buf_uniter(c);

    if (buf_idx == 0)
    {
        free(buf);
        return NULL;
    }

    buf[buf_idx] = '\0';
    return create_token(is_float ? TOK_FLOAT : TOK_INT, buf);
}

Token *MATCH_ident()
{
    char c;
    char *buf = malloc(128);
    if (!buf)
    {
        SET_ERR("Memory allocation failed");
        return NULL;
    }
    int buf_idx = 0;

    while ((c = buf_iter()) != EOF && isalpha(c))
    {
        buf[buf_idx++] = c;
        if (buf_idx == 128)
        {
            SET_ERR("String cannot be longer than 127 characters");
            free(buf);
            return NULL;
        }
    }

    buf_uniter(c);

    if (buf_idx == 0)
    {
        free(buf);
        return NULL;
    }

    buf[buf_idx] = '\0';
    return create_token(TOK_IDENT, buf);
}

Token *MATCH_str()
{
    char c;
    char *buf = malloc(256);
    if (!buf)
    {
        SET_ERR("Memory allocation failed");
        return NULL;
    }
    int buf_idx = 0;

    while ((c = buf_iter()) != EOF && c != '"')
    {
        buf[buf_idx++] = c;

        if (buf_idx == 256)
        {
            SET_ERR("String cannot be longer than 255 characters");
            free(buf);
            return NULL;
        }
    }

    if (c == EOF)
    {
        SET_ERR("String never ends till EOF");
        free(buf);
        return NULL;
    }

    buf[buf_idx] = '\0';
    return create_token(TOK_STRING, buf);
}

Token *MATCH_many(const TokenType exp_token_type, const char *target)
{
    int len = strlen(target);
    char *buf = malloc(len + 1);

    for (int i = 0; i < len; ++i)
    {
        buf[i] = buf_iter();
        if (buf[i] != target[i])
        {
            for (int j = i; j >= 0; --j)
            {
                buf_uniter(buf[j]);
            }
            free(buf);
            return NULL;
        }
    }

    buf[len] = '\0';
    return create_token(exp_token_type, buf);
}

Token *token_next()
{
lexing_next_token:
    char c;
    Token *ret;

    // ignore all white space & comments
    MATCH_wp();

    // ignore comments
    char *from = malloc(3);
    buf_str_peek(2, from);
    if (!strcmp(from, "//"))
    {
        free(from);
        MATCH_inline_comment();
        goto lexing_next_token;
    }
    else if (!strcmp(from, "/*"))
    {
        free(from);
        MATCH_multiline_comment();
        goto lexing_next_token;
    }

#ifdef DEBUG
    char *peek_str = malloc(5);
    buf_str_peek(4, peek_str);

    debug("peek1: '%s'", peek_str);
    free(peek_str);
#endif

    // pay attention to the priority
    switch (c = buf_iter())
    {
    case '+':
        return create_token(TOK_ADD, NULL);
    case '-':
        return create_token(TOK_MIN, NULL);
    case '*':
        return create_token(TOK_MULT, NULL);
    case '/':
        return create_token(TOK_DIV, NULL);
    case '=':
        return create_token(TOK_ASSIGN, NULL);
    case '(':
        return create_token(TOK_LPAREN, NULL);
    case ')':
        return create_token(TOK_RPAREN, NULL);
        break;
    case '{':
        return create_token(TOK_LCBRANCE, NULL);
    case '}':
        return create_token(TOK_RCBRANCE, NULL);
    case ';':
        return create_token(TOK_SEMI, NULL);
    case '"':
        return MATCH_str();
    case EOF:
        return NULL;
    default:
        buf_uniter(c);

        if ((ret = MATCH_many(TOK_LET, "let")))
            return ret;
        if ((ret = MATCH_many(TOK_PRINT, "print")))
            return ret;
        if ((ret = MATCH_many(TOK_IF, "if")))
            return ret;
        if ((ret = MATCH_many(TOK_ELSE, "else")))
            return ret;
        if ((ret = MATCH_many(TOK_TRUE, "true")))
            return ret;
        if ((ret = MATCH_many(TOK_FALSE, "false")))
            return ret;
        if ((ret = MATCH_ident()))
            return ret;
        if ((ret = MATCH_num()))
            return ret;

        // fail to lexing
        char *fail_str_peek = malloc(10);
        buf_str_peek(9, fail_str_peek);
        SET_ERR("unsupported grammar '%s...'", fail_str_peek);
        free(fail_str_peek);

        return NULL;
    }
}

#ifdef LEXING
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("usage: %s <program>\n", argv[0]);
        exit(-1);
    }

    fstream = fopen(argv[1], "r");
    if (!fstream)
    {
        perror("fail to load program");
        exit(-1);
    }

    Token *cur_token;
    do
    {
        cur_token = token_next();
        print_token(cur_token);
    } while (cur_token != NULL);

    if (is_err)
    {
        fprintf(stderr, "Lexing failed: %s\n", err_msg);
        fclose(fstream);
        return -1;
    }
    fclose(fstream);
    printf("done!\n");
    return 0;
}
#endif