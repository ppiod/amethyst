#include <stdlib.h>
#include "astrid_ir.h"

typedef struct {
    IR_Listing* listing;
    int temp_counter;
} IR_Generator;

static void emit(IR_Generator* gen, IR_Op op, IR_Address res, IR_Address arg1, IR_Address arg2) {
    if (gen->listing->count >= gen->listing->capacity) {
        gen->listing->capacity *= 2;
        gen->listing->instructions = realloc(gen->listing->instructions, sizeof(IR_Instruction) * gen->listing->capacity);
    }
    IR_Instruction instr = { .op = op, .result = res, .arg1 = arg1, .arg2 = arg2 };
    gen->listing->instructions[gen->listing->count++] = instr;
}

static IR_Address new_temp(IR_Generator* gen) {
    IR_Address addr = { .type = ADDR_TEMP };
    addr.value.temp_id = gen->temp_counter++;
    return addr;
}

static IR_Address walk_ast(IR_Generator* gen, Expression* node);

static IR_Address walk_infix(IR_Generator* gen, InfixExpression* node) {
    IR_Address left = walk_ast(gen, node->left);
    IR_Address right = walk_ast(gen, node->right);
    IR_Address dest = new_temp(gen);
    
    IR_Op op;
    switch(node->op[0]) {
        case '+': op = IR_ADD; break;
        case '-': op = IR_SUB; break;
        case '*': op = IR_MUL; break;
        case '/': op = IR_DIV; break;
        default:  op = IR_MOV; break;
    }
    
    emit(gen, op, dest, left, right);
    return dest;
}

static IR_Address walk_ast(IR_Generator* gen, Expression* node) {
    switch(node->node.type) {
        case NODE_INTEGER_LITERAL: {
            IR_Address addr = { .type = ADDR_CONST };
            addr.value.const_val = ((IntegerLiteral*)node)->value;
            return addr;
        }
        case NODE_IDENTIFIER: {
            IR_Address addr = { .type = ADDR_VAR };
            addr.value.var_name = ((Identifier*)node)->value;
            return addr;
        }
        case NODE_INFIX_EXPRESSION: {
            return walk_infix(gen, (InfixExpression*)node);
        }
        default: {
            IR_Address empty = { .type = ADDR_EMPTY };
            return empty;
        }
    }
}

IR_Listing* GenerateIR(Program* program) {
    IR_Generator gen;
    gen.listing = malloc(sizeof(IR_Listing));
    gen.listing->capacity = 32;
    gen.listing->instructions = malloc(sizeof(IR_Instruction) * gen.listing->capacity);
    gen.listing->count = 0;
    gen.temp_counter = 0;

    for (int i = 0; i < program->statement_count; i++) {
        Statement* stmt = program->statements[i];
        if (stmt->node.type == NODE_LET_STATEMENT) {
            LetStatement* let = (LetStatement*)stmt;
            IR_Address value = walk_ast(&gen, let->value);
            
            IR_Address dest = { .type = ADDR_VAR };
            dest.value.var_name = let->name->value;
            
            IR_Address empty = { .type = ADDR_EMPTY };
            emit(&gen, IR_MOV, dest, value, empty);
        }
    }
    return gen.listing;
}

void FreeIR(IR_Listing* listing) {
    free(listing->instructions);
    free(listing);
}