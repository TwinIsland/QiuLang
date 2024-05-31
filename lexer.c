#include <common.h>

static char err_msg[128];
static int is_err = 0;

static FILE *fstream;

#define SET_ERR(msg, ...)                     \
    do                                        \
    {                                         \
        sprintf(err_msg, msg, ##__VA_ARGS__); \
        is_err = 1;                           \
    } while (0);

void MATCH_wp(char (*buf_iter)(), void (*buf_uniter)(char))
{
    char c;
    while (c = buf_iter(), (c == ' ' || c == '\t') && c != EOF)
        ;
    buf_uniter(c);
    
}

Token *MATCH_int(char (*buf_iter)(), void (*buf_uniter)(char))
{
    char c;
    int buf_idx = 0;
    char *buf = malloc(64);

    while (c = buf_iter(), (c >= '0' || c <= '9') && c != EOF)
    {
        buf[buf_idx++] = c;
        if (buf_idx == 64)
        {
            SET_ERR("int cannot be larger than 2^64");
            free(buf);
            return NULL;
        }
    };

    if (buf_idx == 0)
    {
        buf_uniter(c);
        return NULL;
    }

    return create_token(TOK_INT, buf);
}

Token *MATCH_ident(char (*buf_iter)(), void (*buf_uniter)(char))
{
    char c = 0;
    char *buf = malloc(128);
    int buf_idx = 0;

    while (c = buf_iter(), (c >= 'a' || c <= 'Z') && c != EOF)
    {
        buf[buf_idx++] = c;
        if (buf_idx == 128)
        {
            SET_ERR("string cannot be longer than 127 character");
            free(buf);
            return NULL;
        }
    }

    if (buf_idx == 0)
    {
        buf_uniter(c);
        return NULL;
    }

    return create_token(TOK_IDENT, buf);
}

Token *MATCH_str(char (*buf_iter)(), void (*buf_uniter)(char))
{
    char c;
    char *buf = malloc(256);
    int buf_idx = 0;

    while ((c = buf_iter()) != '"')
    {
        if (c == EOF)
        {
            SET_ERR("string never ends till eof");
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

Token *MATCH_spec(char (*buf_iter)(), void (*buf_uniter)(char), TokenType exp_token_type, char *target)
{
    int len = strlen(target);
    char *buf = malloc(len + 1);

    for (int i = 0; i < len; ++i)
    {
        buf[i] = buf_iter();
        if (buf[i] != target[i])
        {
            // Undo the changes
            for (int j = i; j >= 0; --j)
            {
                buf_uniter(buf[j]);
            }
            free(buf);
            return NULL;
        }
    }

    buf[len] = '\0'; // Null-terminate the buffer
    return create_token(exp_token_type, buf);
}

Token *token_next(char (*buf_iter)(), void (*buf_uniter)(char))
{
    char c;
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
    case '"':
        return MATCH_str(buf_iter, buf_uniter);
    case EOF:
        return NULL;
    default:
        buf_uniter(c);  // restore the consumed character
        MATCH_wp(buf_iter, buf_uniter); // ignore all white space

        Token *ret;
        if (ret = MATCH_ident(buf_iter, buf_uniter))
            return ret;
        if (ret = MATCH_int(buf_iter, buf_uniter))
            return ret;

        // fail to lexing
        SET_ERR("unsupported grammar.");
        return NULL;
    }
}

char buf_iter()
{
    return fgetc(fstream);
}

void buf_uniter(char ch)
{
    if (ch != '\0')
        ungetc(ch, fstream);
}

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
        cur_token = token_next(buf_iter, buf_uniter);
        print_token(cur_token);
    } while (cur_token != NULL);

    if (is_err)
    {
        fprintf(stderr, "Lexing failed: %s\n", err_msg);
        return -1;
    }
    printf("\ndone!\n");
    return 0;
}