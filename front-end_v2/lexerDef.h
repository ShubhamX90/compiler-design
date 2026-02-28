/**
 * lexerDef.h
 * Contains all data definitions and structures used by the lexical analyzer
 * CS F363 Compiler Design Project
 */

#ifndef LEXERDEF_H
#define LEXERDEF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Buffer size for twin buffer implementation */
#define BUFFER_SIZE 4096
#define TWIN_BUFFER_SIZE (2 * BUFFER_SIZE)
#define MAX_LEXEME_LENGTH 256

/* Token type enumeration - based on Table 1 in language specification */
typedef enum {
    TK_ASSIGNOP,    // <---
    TK_COMMENT,     // %
    TK_FIELDID,     // [a-z][a-z]*
    TK_ID,          // [b-d][2-7][b-d]*[2-7]*
    TK_NUM,         // [0-9][0-9]*
    TK_RNUM,        // [0-9][0-9]*.[0-9][0-9] with optional E[+|-][0-9][0-9]
    TK_FUNID,       // _[a-z|A-Z][a-z|A-Z]*[0-9]*
    TK_RUID,        // #[a-z][a-z]*
    TK_WITH,        // with
    TK_PARAMETERS,  // parameters
    TK_END,         // end
    TK_WHILE,       // while
    TK_UNION,       // union
    TK_ENDUNION,    // endunion
    TK_DEFINETYPE,  // definetype
    TK_AS,          // as
    TK_TYPE,        // type
    TK_MAIN,        // _main
    TK_GLOBAL,      // global
    TK_PARAMETER,   // parameter
    TK_LIST,        // list
    TK_SQL,         // [
    TK_SQR,         // ]
    TK_INPUT,       // input
    TK_OUTPUT,      // output
    TK_INT,         // int
    TK_REAL,        // real
    TK_COMMA,       // ,
    TK_SEM,         // ;
    TK_COLON,       // :
    TK_DOT,         // .
    TK_ENDWHILE,    // endwhile
    TK_OP,          // (
    TK_CL,          // )
    TK_IF,          // if
    TK_THEN,        // then
    TK_ENDIF,       // endif
    TK_READ,        // read
    TK_WRITE,       // write
    TK_RETURN,      // return
    TK_PLUS,        // +
    TK_MINUS,       // -
    TK_MUL,         // *
    TK_DIV,         // /
    TK_CALL,        // call
    TK_RECORD,      // record
    TK_ENDRECORD,   // endrecord
    TK_ELSE,        // else
    TK_AND,         // &&&
    TK_OR,          // @@@
    TK_NOT,         // ~
    TK_LT,          // <
    TK_LE,          // <=
    TK_EQ,          // ==
    TK_GT,          // >
    TK_GE,          // >=
    TK_NE,          // !=
    TK_EOF,         // End of file
    TK_ERROR,       // Lexical error
    TK_EPSILON      // For grammar purposes
} TokenType;

/* Structure to hold token information */
typedef struct {
    TokenType tokenType;
    char lexeme[MAX_LEXEME_LENGTH];
    int lineNumber;
    union {
        int intValue;
        double realValue;
    } value;
    int hasValue;           /* Flag to indicate if value is present */
    int errorType;          /* LexErrorType — valid when tokenType == TK_ERROR */
    char errorMsg[256];     /* Human-readable error description */
} tokenInfo;

/* Twin buffer structure for efficient input handling */
typedef struct {
    char buffer[TWIN_BUFFER_SIZE + 1];  // +1 for EOF markers
    int forward;                         // Forward pointer
    int lexemeBegin;                     // Beginning of current lexeme
    int currentBuffer;                   // 0 or 1 for left/right buffer
    FILE *fp;                           // File pointer
    int eof1, eof2;                     // EOF markers for each half
    int lineNumber;                      // Current line number
} twinBuffer;

/* Keyword structure for lookup table */
typedef struct {
    char *keyword;
    TokenType token;
} KeywordEntry;

/* DFA states - based on DFA design document */
typedef enum {
    S0,   // Start state
    S1,   // Whitespace
    S2,   // Comment
    S11,  // Lowercase word
    S20,  // BCD first letter
    S21,  // Var-ID letter zone
    S22,  // Var-ID digit zone
    S30,  // After underscore
    S31,  // FunID letter zone
    S32,  // FunID digit zone
    S40,  // After hash
    S41,  // RUID body
    S50,  // Integer body
    S51,  // Dot after integer
    S52,  // Real fraction digit 1
    S53,  // Real fraction digit 2
    S54,  // After E
    S55,  // After explicit sign
    S56,  // Exponent digit 1
    S57,  // Exponent digit 2
    S60,  // After <
    S61,  // After <-
    S62,  // After <--
    S63,  // After >
    S64,  // After =
    S65,  // After !
    S66,  // After &
    S67,  // After &&
    S68,  // After @
    S69,  // After @@
    // Accept states
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
    // Error accept states
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

/* Error message structure */
typedef struct {
    int lineNumber;
    char message[256];
} LexError;

/* Error sub-type for TK_ERROR tokens — gives the parser/driver richer info */
typedef enum {
    ERR_NONE = 0,
    ERR_UNKNOWN_SYMBOL,    /* single unrecognised character, e.g. $ @ | */
    ERR_UNKNOWN_PATTERN,   /* multi-char bad pattern, e.g. <--, 5000.7, && */
    ERR_ID_TOO_LONG,       /* variable identifier > 20 chars */
    ERR_FUNID_TOO_LONG     /* function identifier > 30 chars */
} LexErrorType;

#endif /* LEXERDEF_H */
