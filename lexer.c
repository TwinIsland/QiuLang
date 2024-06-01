#include "lexer.h"
#include <ctype.h>

extern FILE *fstream;
// static char (*buf_iter)();
// void (*buf_uniter)(char);

static inline char buf_iter()
{
    return fgetc(fstream);
}

static inline void buf_uniter(char ch)
{
    if (ch != '\0')
        ungetc(ch, fstream);
}

char buf_peek()
{
    char ret = buf_iter();
    buf_uniter(ret);
    return ret;
}

char *buf_str_peek(size_t str_size)
{
    // this function alloc buf on heap, remember to free
    char *ret = malloc(str_size + 1);
    int i = 0;
    for (; i < str_size; ++i)
    {
        ret[i] = buf_peek(buf_iter, buf_uniter);
        if (ret[i] == EOF)
            break;
    }
    ret[i + 1] = '\0';
    return ret;
}

void MATCH_wp()
{
    char c;
    while (c = buf_iter(), (c == ' ' || c == '\t' || c == '\n') && c != EOF)
        ;
    buf_uniter(c);
}

Token *MATCH_int()
{
    char c;
    int buf_idx = 0;
    char *buf = malloc(64);

    while (c = buf_iter(), (isdigit(c) && c != EOF))
    {
        buf[buf_idx++] = c;
        if (buf_idx == 64)
        {
            SET_ERR("int cannot be larger than 2^64");
            free(buf);
            return NULL;
        }
    };

    buf_uniter(c);

    if (buf_idx == 0)
    {
        free(buf);
        return NULL;
    }

    return create_token(TOK_INT, buf);
}

Token *MATCH_ident()
{
    char c = 0;
    char *buf = malloc(128);
    int buf_idx = 0;

    while (c = buf_iter(), (isalpha(c)) && c != EOF)
    {
        buf[buf_idx++] = c;
        if (buf_idx == 128)
        {
            SET_ERR("string cannot be longer than 127 character");
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

    return create_token(TOK_IDENT, buf);
}

Token *MATCH_str()
{
    char c;
    char *buf = malloc(256);
    int buf_idx = 0;

    while ((c = buf_iter()) != '"')
    {
        if (c == EOF)
        {
            SET_ERR("string never ends till eof");
            free(buf);
            return NULL;
        }
        buf[buf_idx++] = c;

        if (buf_idx == 256)
        {
            SET_ERR("string cannot be longer than 255 character");
            free(buf);
            return NULL;
        }
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
    char c;
    MATCH_wp(buf_iter, buf_uniter); // ignore all white space

    debug("peek1: '%c'", buf_peek(buf_iter, buf_uniter));

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

        debug("peek2: %c", buf_peek());

        Token *ret;
        if (ret = MATCH_many(TOK_LET, "let"))
            return ret;
        if (ret = MATCH_many(TOK_PRINT, "print"))
            return ret;
        if (ret = MATCH_many(TOK_IF, "if"))
            return ret;
        if (ret = MATCH_many(TOK_ELSE, "else"))
            return ret;
        if (ret = MATCH_many(TOK_TRUE, "true"))
            return ret;
        if (ret = MATCH_many(TOK_FALSE, "false"))
            return ret;
        if (ret = MATCH_ident())
            return ret;
        if (ret = MATCH_int())
            return ret;

        // fail to lexing
        SET_ERR("unsupported grammar.");
        return NULL;
    }
}

#ifdef LEXER
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