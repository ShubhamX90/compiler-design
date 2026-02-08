/**
 * parserDef.h
 * Parser data structures and grammar definitions
 * CS F363 Compiler Design Project
 */

#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "lexerDef.h"

#define MAX_CHILDREN 10
#define MAX_RHS 15
#define MAX_RULES 100
#define MAX_FIRST_FOLLOW 20

/* Non-terminal symbols */
typedef enum {
    NT_PROGRAM,
    NT_MAINFUNCTION,
    NT_OTHERFUNCTIONS,
    NT_FUNCTION,
    NT_INPUT_PAR,
    NT_OUTPUT_PAR,
    NT_PARAMETER_LIST,
    NT_DATATYPE,
    NT_PRIMITIVEDATATYPE,
    NT_CONSTRUCTEDDATATYPE,
    NT_REMAINING_LIST,
    NT_STMTS,
    NT_TYPEDEFINITIONS,
    NT_TYPEDEFINITION,
    NT_FIELDDEFINITIONS,
    NT_FIELDDEFINITION,
    NT_MOREFIELDS,
    NT_DECLARATIONS,
    NT_DECLARATION,
    NT_GLOBAL_OR_NOT,
    NT_OTHERSTMTS,
    NT_STMT,
    NT_ASSIGNMENTSTMT,
    NT_SINGLEORRECID,
    NT_OPTIONALDOT,
    NT_FUNCALLSTMT,
    NT_OUTPUTPARAMETERS,
    NT_INPUTPARAMETERS,
    NT_ITERATIVESTMT,
    NT_CONDITIONALSTMT,
    NT_IOSTMT,
    NT_ARITHMETICEXPRESSION,
    NT_TERM,
    NT_N7,
    NT_OPERATOR,
    NT_BOOLEANEXPRESSION,
    NT_VAR,
    NT_LOGICALOP,
    NT_RELATIONALOP,
    NT_RETURNSTMT,
    NT_OPTIONALRETURN,
    NT_IDLIST,
    NT_MORE_IDS,
    NT_DEFINETYPESTMT,
    NT_A
} NonTerminal;

/* Symbol type - can be terminal or non-terminal */
typedef enum {
    SYMBOL_TERMINAL,
    SYMBOL_NON_TERMINAL
} SymbolType;

/* Grammar symbol */
typedef struct {
    SymbolType type;
    union {
        TokenType terminal;
        NonTerminal nonTerminal;
    } symbol;
} GrammarSymbol;

/* Grammar rule */
typedef struct {
    NonTerminal lhs;
    GrammarSymbol rhs[MAX_RHS];
    int rhsCount;
    int ruleNumber;
} GrammarRule;

/* Grammar structure */
typedef struct {
    GrammarRule rules[MAX_RULES];
    int ruleCount;
    NonTerminal startSymbol;
} grammar;

/* FIRST set structure */
typedef struct {
    NonTerminal nonTerminal;
    TokenType first[MAX_FIRST_FOLLOW];
    int firstCount;
    int hasEpsilon;
} FirstSet;

/* FOLLOW set structure */
typedef struct {
    NonTerminal nonTerminal;
    TokenType follow[MAX_FIRST_FOLLOW];
    int followCount;
} FollowSet;

/* Combined FIRST and FOLLOW sets */
typedef struct {
    FirstSet firstSets[50];
    int firstCount;
    FollowSet followSets[50];
    int followCount;
} FirstAndFollow;

/* Parse table entry */
typedef struct {
    int isValid;
    int ruleNumber;
    GrammarRule *rule;
} ParseTableEntry;

/* Parse table */
typedef struct {
    ParseTableEntry entries[50][100];  // [NonTerminal][Terminal]
} table;

/* Parse tree node */
typedef struct parseTreeNode {
    GrammarSymbol symbol;
    tokenInfo token;  // Valid for terminal nodes
    struct parseTreeNode *children[MAX_CHILDREN];
    int childCount;
    struct parseTreeNode *parent;
    int isLeaf;
    int visited;
} parseTreeNode;

/* Parse tree */
typedef struct {
    parseTreeNode *root;
} parseTree;

/* Stack node for parsing */
typedef struct stackNode {
    GrammarSymbol symbol;
    parseTreeNode *treeNode;
    struct stackNode *next;
} stackNode;

/* Parser error structure */
typedef struct {
    int lineNumber;
    char message[256];
    TokenType expected[20];
    int expectedCount;
    TokenType received;
} ParseError;

#endif /* PARSERDEF_H */