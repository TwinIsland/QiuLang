# QiuLang
A Turing-complete language implemented entirely in pure C, with no external libraries, built from scratch.

## Run
**Compile lexer:**
```shell
gcc common.c lexer.c parser.c compiler.c -I./ -o compiler -O3
```

for debugging:
```shell
gcc -DDEBUG common.c lexer.c parser.c compiler.c -I./ -o compiler -g
```