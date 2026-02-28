/**
 * lexer.h
 * Function prototypes for the lexical analyzer
 * CS F363 Compiler Design Project
 */

#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

/**
 * Initialize twin buffer structure
 * @param fp File pointer to source code
 * @return Initialized twin buffer
 */
twinBuffer* initializeTwinBuffer(FILE *fp);



/**
 * Get next character from twin buffer
 */
char getNextChar(twinBuffer *tb);

/**
 * Retract forward pointer by n positions
 */
void retract(twinBuffer *tb, int n);

/**
 * Copy current lexeme into caller-supplied buffer
 */
void getLexeme(twinBuffer *tb, char *lexeme);

/**
 * Get next token from the input stream
 */
tokenInfo getNextToken(twinBuffer *tb);

/**
 * Remove comments from source file, write clean code to cleanFile.
 * Line structure (indentation, line numbers) is preserved.
 */
void removeComments(char *testcaseFile, char *cleanFile);

/**
 * Lookup keyword in table; returns TK_FIELDID if not found.
 */
TokenType lookupKeyword(const char *lexeme);

/**
 * Get token name as a printable string.
 */
const char* getTokenName(TokenType token);



/**
 * Free twin buffer and close the underlying file.
 */
void freeTwinBuffer(twinBuffer *tb);

#endif /* LEXER_H */
