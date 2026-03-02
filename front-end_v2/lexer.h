/* Group 10
2022B5A70783P Shiv Tiwari
2022B5A70724P Samyek Jain
2022B3A70551P Ayush Jain
2022B4A71282P Nachiket Garg
2022B5A71326P Mayukh Khetan
2022B5A70763P Shubham Mishra
*/
#ifndef LEXER_H
#define LEXER_H
#include "lexerDef.h"

twinBuffer* initializeTwinBuffer(FILE *fp);

char getNextChar(twinBuffer *tb);

void retract(twinBuffer *tb, int n);

void getLexeme(twinBuffer *tb, char *lexeme);

void freeTwinBuffer(twinBuffer *tb);

TokenType lookupKeyword(const char *lexeme);

tokenInfo getNextToken(twinBuffer *tb);

void removeComments(char *testcaseFile, char *cleanFile);

const char* getTokenName(TokenType token);

#endif
