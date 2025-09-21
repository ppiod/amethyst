#ifndef AMETHYST_TOKEN_H
#define AMETHYST_TOKEN_H

typedef enum {
    TOKEN_ILLEGAL,
    TOKEN_EOF,

    TOKEN_IDENT,
    TOKEN_INT,

    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_ASTERISK,
    TOKEN_SLASH,

    TOKEN_SEMICOLON,

    TOKEN_LPAREN,
    TOKEN_RPAREN,

    TOKEN_LET,
} TokenType;

typedef struct {
    TokenType type;
    char* literal;
} Token;

#endif // AMETHYST_TOKEN_H