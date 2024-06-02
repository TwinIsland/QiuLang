#include "common.h"
#include "lexer.h"
#include "parser.h"

FILE *fstream;
FILE *asm_out_stream;

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