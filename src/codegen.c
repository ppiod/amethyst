#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"

#define CODE_BUFFER_SIZE 8192

static char* my_strdup(const char* s) {
    size_t size = strlen(s) + 1;
    char* p = (char*)malloc(size);
    if (p) {
        memcpy(p, s, size);
    }
    return p;
}

typedef struct {
    char* name;
    int stack_offset;
} VarMap;

typedef struct {
    char buffer[CODE_BUFFER_SIZE];
    IR_Listing* listing;
    VarMap var_map[128];
    int var_count;
    int stack_total_size;
} CodeGenerator;

static int get_stack_offset(CodeGenerator* gen, IR_Address* addr) {
    if (addr->type == ADDR_TEMP) {
        char temp_name[16];
        sprintf(temp_name, "t%d", addr->value.temp_id);
        for (int i = 0; i < gen->var_count; i++) {
            if (strcmp(gen->var_map[i].name, temp_name) == 0) {
                return gen->var_map[i].stack_offset;
            }
        }
    } else if (addr->type == ADDR_VAR) {
        for (int i = 0; i < gen->var_count; i++) {
            if (strcmp(gen->var_map[i].name, addr->value.var_name) == 0) {
                return gen->var_map[i].stack_offset;
            }
        }
    }
    return 0;
}

static void emit(CodeGenerator* gen, const char* code) {
    strcat(gen->buffer, "    ");
    strcat(gen->buffer, code);
    strcat(gen->buffer, "\n");
}

static void generate_instruction(CodeGenerator* gen, IR_Instruction* instr) {
    int res_offset = get_stack_offset(gen, &instr->result);
    int arg1_offset = get_stack_offset(gen, &instr->arg1);
    
    char line[128];

    if (instr->op == IR_MOV) {
        if (instr->arg1.type == ADDR_CONST) {
            sprintf(line, "mov rax, %lld", instr->arg1.value.const_val);
            emit(gen, line);
        } else {
            sprintf(line, "mov rax, [rbp - %d]", arg1_offset);
            emit(gen, line);
        }
        sprintf(line, "mov [rbp - %d], rax", res_offset);
        emit(gen, line);
    } else {
        int arg2_offset = get_stack_offset(gen, &instr->arg2);
        
        if (instr->arg1.type == ADDR_CONST) sprintf(line, "mov rax, %lld", instr->arg1.value.const_val);
        else sprintf(line, "mov rax, [rbp - %d]", arg1_offset);
        emit(gen, line);
        
        if (instr->arg2.type == ADDR_CONST) sprintf(line, "mov rbx, %lld", instr->arg2.value.const_val);
        else sprintf(line, "mov rbx, [rbp - %d]", arg2_offset);
        emit(gen, line);

        switch (instr->op) {
            case IR_ADD: emit(gen, "add rax, rbx"); break;
            case IR_SUB: emit(gen, "sub rax, rbx"); break;
            case IR_MUL: emit(gen, "imul rax, rbx"); break;
            case IR_DIV: emit(gen, "cqo"); emit(gen, "idiv rbx"); break;
            default: break;
        }
        
        sprintf(line, "mov [rbp - %d], rax", res_offset);
        emit(gen, line);
    }
}

char* GenerateAssembly(IR_Listing* listing) {
    CodeGenerator gen = {0};
    gen.listing = listing;
    gen.stack_total_size = 0;
    gen.var_count = 0;

    for (int i = 0; i < listing->count; i++) {
        IR_Instruction* instr = &listing->instructions[i];
        IR_Address* operands[] = {&instr->result, &instr->arg1, &instr->arg2};
        for (int j = 0; j < 3; j++) {
            IR_Address* addr = operands[j];
            if ((addr->type == ADDR_VAR || addr->type == ADDR_TEMP) && get_stack_offset(&gen, addr) == 0) {
                gen.stack_total_size += 8;
                gen.var_map[gen.var_count].stack_offset = gen.stack_total_size;
                
                char name_buffer[16];
                if (addr->type == ADDR_VAR) strcpy(name_buffer, addr->value.var_name);
                else sprintf(name_buffer, "t%d", addr->value.temp_id);
                
                gen.var_map[gen.var_count].name = my_strdup(name_buffer);
                gen.var_count++;
            }
        }
    }

    strcpy(gen.buffer, "section .text\n");
    strcat(gen.buffer, "global _start\n\n");
    
    strcat(gen.buffer, "_print_and_newline:\n");
    strcat(gen.buffer, "    push rbp\n");
    strcat(gen.buffer, "    mov rbp, rsp\n");
    strcat(gen.buffer, "    sub rsp, 32\n");
    strcat(gen.buffer, "    mov rax, rdi\n");
    strcat(gen.buffer, "    mov rbx, 10\n");
    strcat(gen.buffer, "    lea rcx, [rbp - 1]\n");
    strcat(gen.buffer, "    mov byte [rcx], 10\n");
    strcat(gen.buffer, ".print_loop:\n");
    strcat(gen.buffer, "    xor rdx, rdx\n");
    strcat(gen.buffer, "    div rbx\n");
    strcat(gen.buffer, "    add rdx, 48\n");
    strcat(gen.buffer, "    dec rcx\n");
    strcat(gen.buffer, "    mov [rcx], dl\n");
    strcat(gen.buffer, "    test rax, rax\n");
    strcat(gen.buffer, "    jnz .print_loop\n");
    strcat(gen.buffer, ".print_write:\n");
    strcat(gen.buffer, "    mov rdi, 1\n");
    strcat(gen.buffer, "    mov rsi, rcx\n");
    strcat(gen.buffer, "    mov rdx, rbp\n");
    strcat(gen.buffer, "    sub rdx, rcx\n");
    strcat(gen.buffer, "    mov rax, 1\n");
    strcat(gen.buffer, "    syscall\n");
    strcat(gen.buffer, "    mov rsp, rbp\n");
    strcat(gen.buffer, "    pop rbp\n");
    strcat(gen.buffer, "    ret\n\n");

    strcat(gen.buffer, "_start:\n");
    
    emit(&gen, "push rbp");
    emit(&gen, "mov rbp, rsp");
    char stack_alloc[128];
    sprintf(stack_alloc, "sub rsp, %d", gen.stack_total_size);
    emit(&gen, stack_alloc);

    for (int i = 0; i < listing->count; i++) {
        generate_instruction(&gen, &listing->instructions[i]);
    }
    
    IR_Address* final_result_addr = &listing->instructions[listing->count - 1].result;
    sprintf(stack_alloc, "mov rdi, [rbp - %d]", get_stack_offset(&gen, final_result_addr));
    emit(&gen, stack_alloc);
    
    emit(&gen, "call _print_and_newline");

    emit(&gen, "mov rsp, rbp");
    emit(&gen, "pop rbp");
    emit(&gen, "mov rax, 60");
    emit(&gen, "xor rdi, rdi");
    emit(&gen, "syscall");

    for(int i = 0; i < gen.var_count; i++) free(gen.var_map[i].name);

    return my_strdup(gen.buffer);
}