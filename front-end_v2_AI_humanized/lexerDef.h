/* lexerDef.h - data definitions for the lexer */

#ifndef LEXERDEF_H
#define LEXERDEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// twin buffer: each half is 4096 bytes
#define BUFFER_SIZE 4096
#define TWIN_BUFFER_SIZE (2 * BUFFER_SIZE)
#define MAX_LEXEME_LENGTH 256

/* token types - see Table 1 in the spec */
typedef enum {
    TK_ASSIGNOP,    // <---
    TK_COMMENT,     // %
    TK_FIELDID,     // [a-z][a-z]*
    TK_ID,          // [b-d][2-7][b-d]*[2-7]*
    TK_NUM,         // digits
    TK_RNUM,        // real number, e.g. 12.34 or 12.34E+01
    TK_FUNID,       // _[a-zA-Z]+[0-9]*
    TK_RUID,        // #[a-z]+
    TK_WITH,
    TK_PARAMETERS,
    TK_END,
    TK_WHILE,
    TK_UNION,
    TK_ENDUNION,
    TK_DEFINETYPE,
    TK_AS,
    TK_TYPE,
    TK_MAIN,        // _main
    TK_GLOBAL,
    TK_PARAMETER,
    TK_LIST,
    TK_SQL,         // [
    TK_SQR,         // ]
    TK_INPUT,
    TK_OUTPUT,
    TK_INT,
    TK_REAL,
    TK_COMMA,
    TK_SEM,
    TK_COLON,
    TK_DOT,
    TK_ENDWHILE,
    TK_OP,          // (
    TK_CL,          // )
    TK_IF,
    TK_THEN,
    TK_ENDIF,
    TK_READ,
    TK_WRITE,
    TK_RETURN,
    TK_PLUS,
    TK_MINUS,
    TK_MUL,
    TK_DIV,
    TK_CALL,
    TK_RECORD,
    TK_ENDRECORD,
    TK_ELSE,
    TK_AND,         // &&&
    TK_OR,          // @@@
    TK_NOT,         // ~
    TK_LT,
    TK_LE,
    TK_EQ,
    TK_GT,
    TK_GE,
    TK_NE,
    TK_EOF,
    TK_ERROR,
    TK_EPSILON      // used internally for grammar epsilon productions
} TokenType;

/* all the info we need about one token */
typedef struct {
    TokenType tokenType;
    char lexeme[MAX_LEXEME_LENGTH];
    int lineNumber;
    union {
        int intValue;
        double realValue;
    } value;
    int hasValue;
    int errorType;          /* which kind of lex error (0 = none) */
    char errorMsg[256];
} tokenInfo;

/* twin buffer structure */
typedef struct {
    char buffer[TWIN_BUFFER_SIZE + 1];
    int forward;
    int lexemeBegin;
    int currentBuffer;      // 0 = left half, 1 = right half
    FILE *fp;
    int eof1, eof2;         // EOF flags for each half
    int lineNumber;
} twinBuffer;

/* one entry in the keyword table */
typedef struct {
    char *keyword;
    TokenType token;
} KeywordEntry;

/* DFA states */
typedef enum {
    S0,   // start
    S1,   // whitespace
    S2,   // comment
    S11,  // lowercase word
    S20,  // b/c/d seen
    S21,  // var-id letter part
    S22,  // var-id digit part
    S30,  // after underscore
    S31,  // funid letters
    S32,  // funid digits
    S40,  // after #
    S41,  // ruid body
    S50,  // integer digits
    S51,  // dot after integer
    S52,  // first fraction digit
    S53,  // second fraction digit
    S54,  // after E
    S55,  // after sign
    S56,  // first exponent digit
    S57,  // second exponent digit
    S60,  // after <
    S61,  // after <-
    S62,  // after <--
    S63,  // after >
    S64,  // after =
    S65,  // after !
    S66,  // after &
    S67,  // after &&
    S68,  // after @
    S69,  // after @@
    // accept states
    A_SKIP_WS,
    A_SKIP_COMMENT,
    A_WORD,
    A_VARID,
    A_FUNID,
    A_RUID,
    A_INT,
    A_ERR_INTDOT,
    A_REAL,
    A_REAL_EXP,
    A_ASSIGNOP,
    A_LT, A_LE, A_GT, A_GE, A_EQ, A_NE,
    A_AND, A_OR, A_NOT,
    A_PLUS, A_MINUS, A_MUL, A_DIV,
    A_COMMA, A_SEM, A_COLON, A_DOT,
    A_OP, A_CL,
    A_SQL, A_SQR,
    A_EOF,
    // error accept states
    A_ERR_CHAR,
    A_ERR_FUN,
    A_ERR_RUID,
    A_ERR_RNUM_1DIG,
    A_ERR_EXP_NOSIGN,
    A_ERR_EXP_NODIG1,
    A_ERR_EXP_1DIG,
    A_ERR_EQ,
    A_ERR_BANG,
    A_ERR_AMP,
    A_ERR_AND2,
    A_ERR_AT,
    A_ERR_AT2,
    A_ERR_LTMINUS,
    A_ERR_LTDOUBLE
} DFAState;

typedef struct {
    int lineNumber;
    char message[256];
} LexError;

/* error sub-types stored in tokenInfo.errorType */
typedef enum {
    ERR_NONE = 0,
    ERR_UNKNOWN_SYMBOL,
    ERR_UNKNOWN_PATTERN,
    ERR_ID_TOO_LONG,
    ERR_FUNID_TOO_LONG
} LexErrorType;

#endif /* LEXERDEF_H */
