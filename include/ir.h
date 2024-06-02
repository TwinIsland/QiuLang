#pragma once

typedef enum
{
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_PUSH,
    IR_ASSIGN,
    IR_PRINT,
    IR_JMP_IF_FALSE,
    IR_LABEL,
    IR_NOP
} IRType;

typedef struct
{
    IRType type;
    double value;   // IR_PUSH
    char ident[32]; // identifiers
    int label;      // jump labels
} IRInstruction;

#define MAX_IR_INSTRUCTIONS 1024

void ir_init();
void ir_emit(IRType type, double value, const char *ident, int label);
void ir_print();
void ir_generate_asm();
