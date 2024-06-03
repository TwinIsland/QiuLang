# QiuLang
A Turing-complete language implemented entirely in pure C, with no external libraries, built from scratch.

![compiling pipline](docs/pipline.svg)

## Run
**Compile lexer:**
```shell
gcc *.c -Iinclude -o qiu -O3 -Wall
```

for debugging:
```shell
gcc -DDEBUG *.c -Iinclude -o qiu -g -Wall
```