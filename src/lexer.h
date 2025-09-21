#ifndef AMETHYST_LEXER_H
#define AMETHYST_LEXER_H

#include "token.h"
#include <stddef.h>

typedef struct {
    const char* input;
    size_t position;
    size_t readPosition;
    char ch;
} Lexer;

Lexer* NewLexer(const char* input);
Token NextToken(Lexer* l);
void FreeLexer(Lexer* l);

#endif // AMETHYST_LEXER_H