/* lexer.h - function prototypes for the lexer */

#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

twinBuffer *initializeTwinBuffer(FILE *fp);
char getNextChar(twinBuffer *tb);
void retract(twinBuffer *tb, int n);
void getLexeme(twinBuffer *tb, char *lexeme);
tokenInfo getNextToken(twinBuffer *tb);

// strips comments from file, writes clean version to cleanFile
// blank lines are preserved so line numbers stay correct
void removeComments(char *testcaseFile, char *cleanFile);

TokenType lookupKeyword(const char *lexeme);
const char *getTokenName(TokenType token);
void freeTwinBuffer(twinBuffer *tb);

#endif /* LEXER_H */
