#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"

static void readChar(Lexer* l) {
    if (l->readPosition >= strlen(l->input)) {
        l->ch = 0;
    } else {
        l->ch = l->input[l->readPosition];
    }
    l->position = l->readPosition;
    l->readPosition += 1;
}

Lexer* NewLexer(const char* input) {
    Lexer* l = (Lexer*)malloc(sizeof(Lexer));
    l->input = input;
    l->position = 0;
    l->readPosition = 0;
    l->ch = 0;
    readChar(l);
    return l;
}

void FreeLexer(Lexer* l) {
    free(l);
}

static void skipWhitespace(Lexer* l) {
    while (l->ch == ' ' || l->ch == '\t' || l->ch == '\n' || l->ch == '\r') {
        readChar(l);
    }
}

static char* readIdentifier(Lexer* l) {
    size_t startPosition = l->position;
    while (isalpha(l->ch) || l->ch == '_') {
        readChar(l);
    }
    size_t length = l->position - startPosition;
    char* ident = (char*)malloc(length + 1);
    strncpy(ident, l->input + startPosition, length);
    ident[length] = '\0';
    return ident;
}

static char* readNumber(Lexer* l) {
    size_t startPosition = l->position;
    while (isdigit(l->ch)) {
        readChar(l);
    }
    size_t length = l->position - startPosition;
    char* num = (char*)malloc(length + 1);
    strncpy(num, l->input + startPosition, length);
    num[length] = '\0';
    return num;
}

static TokenType lookupIdent(const char* ident) {
    if (strcmp(ident, "let") == 0) return TOKEN_LET;
    return TOKEN_IDENT;
}

static Token newToken(TokenType type, char* literal) {
    Token tok;
    tok.type = type;
    tok.literal = literal;
    return tok;
}

Token NextToken(Lexer* l) {
    skipWhitespace(l);

    Token tok;
    char* lit = (char*)malloc(2);
    lit[0] = l->ch;
    lit[1] = '\0';

    switch (l->ch) {
        case '=': tok = newToken(TOKEN_ASSIGN, lit); break;
        case '+': tok = newToken(TOKEN_PLUS, lit); break;
        case '-': tok = newToken(TOKEN_MINUS, lit); break;
        case '*': tok = newToken(TOKEN_ASTERISK, lit); break;
        case '/': tok = newToken(TOKEN_SLASH, lit); break;
        case '(': tok = newToken(TOKEN_LPAREN, lit); break;
        case ')': tok = newToken(TOKEN_RPAREN, lit); break;
        case ';': tok = newToken(TOKEN_SEMICOLON, lit); break;
        case 0:
            free(lit);
            tok = newToken(TOKEN_EOF, "");
            break;
        default:
            free(lit);
            if (isalpha(l->ch) || l->ch == '_') {
                char* identifier = readIdentifier(l);
                tok.type = lookupIdent(identifier);
                tok.literal = identifier;
                return tok;
            } else if (isdigit(l->ch)) {
                tok.type = TOKEN_INT;
                tok.literal = readNumber(l);
                return tok;
            } else {
                lit = (char*)malloc(2);
                lit[0] = l->ch;
                lit[1] = '\0';
                tok = newToken(TOKEN_ILLEGAL, lit);
            }
            break;
    }

    readChar(l);
    return tok;
}