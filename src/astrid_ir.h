#ifndef AMETHYST_ASTRID_IR_H
#define AMETHYST_ASTRID_IR_H

#include "ast.h"

typedef enum {
    IR_ADD,
    IR_SUB,
    IR_MUL,
    IR_DIV,
    IR_MOV,
} IR_Op;

typedef enum {
    ADDR_EMPTY,
    ADDR_CONST,
    ADDR_VAR,
    ADDR_TEMP,
} IR_AddrType;

typedef struct {
    IR_AddrType type;
    union {
        long long const_val;
        char* var_name;
        int temp_id;
    } value;
} IR_Address;

typedef struct {
    IR_Op op;
    IR_Address result;
    IR_Address arg1;
    IR_Address arg2;
} IR_Instruction;

typedef struct {
    IR_Instruction* instructions;
    int count;
    int capacity;
} IR_Listing;

IR_Listing* GenerateIR(Program* program);
void FreeIR(IR_Listing* listing);

#endif // AMETHYST_ASTRID_IR_H