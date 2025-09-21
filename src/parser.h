#ifndef AMETHYST_PARSER_H
#define AMETHYST_PARSER_H

#include "lexer.h"
#include "ast.h"

typedef struct Parser Parser;

typedef Expression* (*prefixParseFn)(Parser*);
typedef Expression* (*infixParseFn)(Parser*, Expression*);

struct Parser {
    Lexer* l;
    Token currentToken;
    Token peekToken;
    prefixParseFn prefixParseFns[TOKEN_LET + 1];
    infixParseFn infixParseFns[TOKEN_LET + 1];
};

Parser* NewParser(Lexer* l);
void FreeParser(Parser* p);
Program* ParseProgram(Parser* p);

#endif // AMETHYST_PARSER_H