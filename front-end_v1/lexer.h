/**
 * lexer.h
 * Function prototypes for the lexical analyzer
 * CS F363 Compiler Design Project
 */

#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

/**
 * Initialize and return a twin buffer for efficient input streaming
 * @param fp File pointer to the source code
 * @return Pointer to initialized twin buffer
 */
FILE* getStream(FILE *fp);

/**
 * Initialize twin buffer structure
 * @param fp File pointer to source code
 * @return Initialized twin buffer
 */
twinBuffer* initializeTwinBuffer(FILE *fp);

/**
 * Get next character from twin buffer
 * @param tb Twin buffer
 * @return Next character or EOF
 */
char getNextChar(twinBuffer *tb);

/**
 * Retract forward pointer by n positions
 * @param tb Twin buffer
 * @param n Number of positions to retract
 */
void retract(twinBuffer *tb, int n);

/**
 * Get the current lexeme from twin buffer
 * @param tb Twin buffer
 * @param lexeme Buffer to store lexeme
 */
void getLexeme(twinBuffer *tb, char *lexeme);

/**
 * Get next token from the input stream
 * @param tb Twin buffer
 * @return Token information structure
 */
tokenInfo getNextToken(twinBuffer *tb);

/**
 * Remove comments from source file and write clean code to output file
 * @param testcaseFile Input source file
 * @param cleanFile Output file with comments removed
 */
void removeComments(char *testcaseFile, char *cleanFile);

/**
 * Lookup keyword in keyword table
 * @param lexeme String to lookup
 * @return Token type if keyword, TK_FIELDID otherwise
 */
TokenType lookupKeyword(const char *lexeme);

/**
 * Get token name as string
 * @param token Token type
 * @return String representation of token
 */
const char* getTokenName(TokenType token);

/**
 * Check if character is a digit 0-9
 * @param c Character to check
 * @return 1 if digit, 0 otherwise
 */
int isDIG(char c);

/**
 * Check if character is a digit 2-7
 * @param c Character to check
 * @return 1 if digit 2-7, 0 otherwise
 */
int isD27(char c);

/**
 * Check if character is lowercase letter
 * @param c Character to check
 * @return 1 if lowercase, 0 otherwise
 */
int isLOW(char c);

/**
 * Check if character is b, c, or d
 * @param c Character to check
 * @return 1 if b/c/d, 0 otherwise
 */
int isBD(char c);

/**
 * Check if character is any letter (upper or lowercase)
 * @param c Character to check
 * @return 1 if letter, 0 otherwise
 */
int isLET(char c);

/**
 * Check if character is whitespace
 * @param c Character to check
 * @return 1 if whitespace, 0 otherwise
 */
int isWS(char c);

/**
 * Print token information (for testing)
 * @param token Token information
 */
void printToken(tokenInfo token);

/**
 * Free twin buffer resources
 * @param tb Twin buffer to free
 */
void freeTwinBuffer(twinBuffer *tb);

#endif /* LEXER_H */
