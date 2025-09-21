#ifndef AMETHYST_AST_H
#define AMETHYST_AST_H

#include "token.h"
#include <stdio.h>

typedef enum {
    NODE_PROGRAM,
    NODE_LET_STATEMENT,
    NODE_IDENTIFIER,
    NODE_INTEGER_LITERAL,
    NODE_INFIX_EXPRESSION,
} NodeType;

typedef struct Node {
    NodeType type;
} Node;

typedef struct Expression {
    Node node;
    Token token;
} Expression;

typedef struct Statement {
    Node node;
    Token token;
} Statement;

typedef struct Identifier {
    Expression expression;
    char* value;
} Identifier;

typedef struct IntegerLiteral {
    Expression expression;
    long long value;
} IntegerLiteral;

typedef struct LetStatement {
    Statement statement;
    Identifier* name;
    Expression* value;
} LetStatement;

typedef struct InfixExpression {
    Expression expression;
    Expression* left;
    char* op;
    Expression* right;
} InfixExpression;

typedef struct Program {
    Node node;
    Statement** statements;
    int statement_count;
    int statement_capacity;
} Program;

Program* NewProgram();
void FreeNode(Node* node);

#endif // AMETHYST_AST_H