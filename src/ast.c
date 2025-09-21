#include <stdlib.h>
#include "ast.h"

Program* NewProgram() {
    Program* prog = (Program*)calloc(1, sizeof(Program));
    prog->node.type = NODE_PROGRAM;
    prog->statement_capacity = 8;
    prog->statements = (Statement**)malloc(sizeof(Statement*) * prog->statement_capacity);
    return prog;
}

void FreeNode(Node* node) {
    if (!node) return;
    switch (node->type) {
        case NODE_PROGRAM: {
            Program* p = (Program*)node;
            for (int i = 0; i < p->statement_count; i++) FreeNode((Node*)p->statements[i]);
            free(p->statements);
            break;
        }
        case NODE_LET_STATEMENT: {
            LetStatement* s = (LetStatement*)node;
            free(s->statement.token.literal);
            FreeNode((Node*)s->name);
            FreeNode((Node*)s->value);
            break;
        }
        case NODE_IDENTIFIER: {
            Identifier* i = (Identifier*)node;
            free(i->value);
            break;
        }
        case NODE_INFIX_EXPRESSION: {
            InfixExpression* e = (InfixExpression*)node;
            free(e->op);
            FreeNode((Node*)e->left);
            FreeNode((Node*)e->right);
            break;
        }
        case NODE_INTEGER_LITERAL: break;
    }
    free(node);
}