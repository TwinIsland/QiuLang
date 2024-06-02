#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ir.h"

static IRInstruction ir_instructions[MAX_IR_INSTRUCTIONS];
static int ir_index = 0;

void ir_init()
{
    ir_index = 0;
}

void ir_emit(IRType type, double value, const char *ident, int label)
{
    if (ir_index >= MAX_IR_INSTRUCTIONS)
    {
        fprintf(stderr, "IR buffer overflow\n");
        exit(EXIT_FAILURE);
    }
    IRInstruction inst = {type, value, "", label};
    if (ident)
    {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
        strncpy(inst.ident, ident, sizeof(inst.ident) - 1);
#pragma GCC diagnostic pop
    }
    ir_instructions[ir_index++] = inst;
}

void ir_print()
{
    for (int i = 0; i < ir_index; i++)
    {
        switch (ir_instructions[i].type)
        {
        case IR_ADD:
            printf("ADD\n");
            break;
        case IR_SUB:
            printf("SUB\n");
            break;
        case IR_MUL:
            printf("MUL\n");
            break;
        case IR_DIV:
            printf("DIV\n");
            break;
        case IR_PUSH:
            printf("PUSH %f\n", ir_instructions[i].value);
            break;
        case IR_ASSIGN:
            printf("ASSIGN %s\n", ir_instructions[i].ident);
            break;
        case IR_PRINT:
            printf("PRINT %s\n", ir_instructions[i].ident);
            break;
        case IR_JMP_IF_FALSE:
            printf("JMP_IF_FALSE L%d\n", ir_instructions[i].label);
            break;
        case IR_LABEL:
            printf("LABEL L%d\n", ir_instructions[i].label);
            break;
        case IR_NOP:
            printf("NOP\n");
            break;
        default:
            break;
        }
    }
}

void ir_generate_asm()
{
    for (int i = 0; i < ir_index; i++)
    {
        switch (ir_instructions[i].type)
        {
        case IR_ADD:
            printf("POP R1\n");
            printf("POP R0\n");
            printf("ADD R0, R1\n");
            printf("PUSH R0\n");
            break;
        case IR_SUB:
            printf("POP R1\n");
            printf("POP R0\n");
            printf("SUB R0, R1\n");
            printf("PUSH R0\n");
            break;
        case IR_MUL:
            printf("POP R1\n");
            printf("POP R0\n");
            printf("MUL R0, R1\n");
            printf("PUSH R0\n");
            break;
        case IR_DIV:
            printf("POP R1\n");
            printf("POP R0\n");
            printf("DIV R0, R1\n");
            printf("PUSH R0\n");
            break;
        case IR_PUSH:
            printf("PUSH %f\n", ir_instructions[i].value);
            break;
        case IR_ASSIGN:
            printf("POP %s\n", ir_instructions[i].ident);
            break;
        case IR_PRINT:
            printf("PRINT %s\n", ir_instructions[i].ident);
            break;
        case IR_JMP_IF_FALSE:
            printf("POP R0\n");
            printf("CMP R0, 0\n");
            printf("JZ L%d\n", ir_instructions[i].label);
            break;
        case IR_LABEL:
            printf("L%d:\n", ir_instructions[i].label);
            break;
        case IR_NOP:
            printf("NOP\n");
            break;
        default:
            break;
        }
    }
}
