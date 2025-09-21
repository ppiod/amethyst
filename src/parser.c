#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "parser.h"

typedef enum {
    PREC_LOWEST,
    PREC_SUM,
    PREC_PRODUCT,
} Precedence;

static Precedence precedences[] = {
    [TOKEN_PLUS]     = PREC_SUM,
    [TOKEN_MINUS]    = PREC_SUM,
    [TOKEN_SLASH]    = PREC_PRODUCT,
    [TOKEN_ASTERISK] = PREC_PRODUCT,
};

static void nextToken(Parser* p);
static Expression* parseExpression(Parser* p, Precedence precedence);
static Expression* parseIdentifier(Parser* p);
static Expression* parseIntegerLiteral(Parser* p);
static Expression* parseInfixExpression(Parser* p, Expression* left);
static Expression* parseGroupedExpression(Parser* p);

static Precedence peekPrecedence(Parser* p) {
    if (p->peekToken.type > TOKEN_LET || precedences[p->peekToken.type] == 0) {
        return PREC_LOWEST;
    }
    return precedences[p->peekToken.type];
}

void registerPrefix(Parser* p, TokenType type, prefixParseFn fn) {
    p->prefixParseFns[type] = fn;
}

void registerInfix(Parser* p, TokenType type, infixParseFn fn) {
    p->infixParseFns[type] = fn;
}

Parser* NewParser(Lexer* l) {
    Parser* p = (Parser*)calloc(1, sizeof(Parser));
    p->l = l;

    registerPrefix(p, TOKEN_IDENT, parseIdentifier);
    registerPrefix(p, TOKEN_INT, parseIntegerLiteral);
    registerPrefix(p, TOKEN_LPAREN, parseGroupedExpression);
    
    registerInfix(p, TOKEN_PLUS, parseInfixExpression);
    registerInfix(p, TOKEN_MINUS, parseInfixExpression);
    registerInfix(p, TOKEN_SLASH, parseInfixExpression);
    registerInfix(p, TOKEN_ASTERISK, parseInfixExpression);

    nextToken(p);
    nextToken(p);
    
    return p;
}

void FreeParser(Parser* p) {
    free(p);
}

static void nextToken(Parser* p) {
    p->currentToken = p->peekToken;
    p->peekToken = NextToken(p->l);
}

static LetStatement* parseLetStatement(Parser* p) {
    LetStatement* stmt = (LetStatement*)calloc(1, sizeof(LetStatement));
    stmt->statement.node.type = NODE_LET_STATEMENT;
    stmt->statement.token = p->currentToken;

    if (p->peekToken.type != TOKEN_IDENT) {
        free(stmt); 
        return NULL;
    }
    nextToken(p);

    Identifier* ident = (Identifier*)calloc(1, sizeof(Identifier));
    ident->expression.node.type = NODE_IDENTIFIER;
    ident->expression.token = p->currentToken;
    ident->value = p->currentToken.literal;
    stmt->name = ident;

    if (p->peekToken.type != TOKEN_ASSIGN) {
        free(ident); 
        free(stmt); 
        return NULL;
    }
    nextToken(p);
    nextToken(p);

    stmt->value = parseExpression(p, PREC_LOWEST);

    if (p->peekToken.type == TOKEN_SEMICOLON) {
        nextToken(p);
    }
    
    return stmt;
}

Program* ParseProgram(Parser* p) {
    Program* program = NewProgram();

    while (p->currentToken.type != TOKEN_EOF) {
        Statement* stmt = NULL;
        if (p->currentToken.type == TOKEN_LET) {
            stmt = (Statement*)parseLetStatement(p);
        }

        if (stmt != NULL) {
            if (program->statement_count >= program->statement_capacity) {
                program->statement_capacity *= 2;
                program->statements = (Statement**)realloc(program->statements, sizeof(Statement*) * program->statement_capacity);
            }
            program->statements[program->statement_count] = stmt;
            program->statement_count++;
        }
        nextToken(p);
    }
    return program;
}

static Expression* parseExpression(Parser* p, Precedence precedence) {
    prefixParseFn prefix = p->prefixParseFns[p->currentToken.type];
    if (prefix == NULL) {
        return NULL;
    }
    Expression* leftExp = prefix(p);

    while (p->peekToken.type != TOKEN_SEMICOLON && precedence < peekPrecedence(p)) {
        infixParseFn infix = p->infixParseFns[p->peekToken.type];
        if (infix == NULL) {
            return leftExp;
        }
        nextToken(p);
        leftExp = infix(p, leftExp);
    }

    return leftExp;
}

static Expression* parseIdentifier(Parser* p) {
    Identifier* ident = (Identifier*)calloc(1, sizeof(Identifier));
    ident->expression.node.type = NODE_IDENTIFIER;
    ident->expression.token = p->currentToken;
    ident->value = p->currentToken.literal;
    return (Expression*)ident;
}

static Expression* parseIntegerLiteral(Parser* p) {
    IntegerLiteral* lit = (IntegerLiteral*)calloc(1, sizeof(IntegerLiteral));
    lit->expression.node.type = NODE_INTEGER_LITERAL;
    lit->expression.token = p->currentToken;
    lit->value = strtoll(p->currentToken.literal, NULL, 10);
    free(p->currentToken.literal);
    return (Expression*)lit;
}

static Expression* parseInfixExpression(Parser* p, Expression* left) {
    InfixExpression* exp = (InfixExpression*)calloc(1, sizeof(InfixExpression));
    exp->expression.node.type = NODE_INFIX_EXPRESSION;
    exp->expression.token = p->currentToken;
    exp->op = p->currentToken.literal;
    exp->left = left;

    Precedence prec = precedences[p->currentToken.type];
    nextToken(p);
    exp->right = parseExpression(p, prec);
    
    return (Expression*)exp;
}

static Expression* parseGroupedExpression(Parser* p) {
    nextToken(p);
    
    Expression* exp = parseExpression(p, PREC_LOWEST);
    
    if (p->peekToken.type != TOKEN_RPAREN) {
        return NULL;
    }
    nextToken(p);
    
    return exp;
}