/**
 * parserDef.h
 * Parser data structures and grammar definitions
 * CS F363 Compiler Design Project - BITS Pilani
 *
 * Non-terminal set updated per Modified LL(1) Grammar (Feb 13, 2026):
 *  - NT_ACTUALORREDEFINED   : for typeDefinitions (13-a/b)
 *  - NT_FIELDTYPE           : for fieldDefinition  (17-a)
 *  - NT_OPTION_SINGLE_CONSTRUCTED, NT_ONEEXPANSION, NT_MOREEXPANSIONS
 *                           : for singleOrRecId    (25-c/d/e/f)
 *  - NT_ELSEPART            : for conditionalStmt  (31-a)
 *  - NT_EXPPRIME, NT_TERMPRIME, NT_FACTOR,
 *    NT_HIGHPRECEDENCEOP, NT_LOWPRECEDENCEOP
 *                           : arithmetic with precedence (B1–B7)
 */

#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "lexerDef.h"

/* ---- Sizes ---- */
#define MAX_CHILDREN 15
#define MAX_RHS 15
#define MAX_RULES 140
#define MAX_FIRST_FOLLOW 45 /* enough for large FOLLOW sets */
#define MAX_NT 60           /* slightly more than NT_COUNT */
#define TABLE_TERMINALS 70  /* more than TK_EPSILON value */

/* -----------------------------------------------
 * Non-terminal enumeration
 * Ordering must match NT_COUNT at the end.
 * ----------------------------------------------- */
typedef enum {
  NT_PROGRAM = 0,
  NT_MAINFUNCTION,
  NT_OTHERFUNCTIONS,
  NT_FUNCTION,
  NT_INPUT_PAR,
  NT_OUTPUT_PAR,
  NT_PARAMETER_LIST,
  NT_DATATYPE,
  NT_PRIMITIVEDATATYPE,
  NT_CONSTRUCTEDDATATYPE, /* 9  – includes TK_RUID alternative (rule 10-a) */
  NT_REMAINING_LIST,
  NT_STMTS,
  NT_TYPEDEFINITIONS,
  NT_ACTUALORREDEFINED, /* 13 – NEW: <typeDefinition> | <definetypestmt> */
  NT_TYPEDEFINITION,
  NT_FIELDDEFINITIONS,
  NT_FIELDTYPE, /* 16 – NEW: primitiveDatatype | constructedDatatype */
  NT_FIELDDEFINITION,
  NT_MOREFIELDS,
  NT_DECLARATIONS,
  NT_DECLARATION,   /* 20 */
  NT_GLOBAL_OR_NOT, /* 21 – now: TK_COLON TK_GLOBAL | eps */
  NT_OTHERSTMTS,
  NT_STMT,
  NT_ASSIGNMENTSTMT,
  NT_SINGLEORRECID,             /* 25e */
  NT_OPTION_SINGLE_CONSTRUCTED, /* 25f – eps | <oneExpansion><moreExpansions> */
  NT_ONEEXPANSION,              /* 25c – TK_DOT TK_FIELDID */
  NT_MOREEXPANSIONS,            /* 25d – <oneExpansion><moreExpansions> | eps */
  NT_FUNCALLSTMT,
  NT_OUTPUTPARAMETERS, /* 30 */
  NT_INPUTPARAMETERS,
  NT_ITERATIVESTMT,
  NT_CONDITIONALSTMT, /* 30-a */
  NT_ELSEPART,        /* 31-a – TK_ELSE <stmts> TK_ENDIF | TK_ENDIF */
  NT_IOSTMT,
  NT_ARITHMETICEXPRESSION, /* B1 */
  NT_EXPPRIME,             /* B2 – lowPrecOp term expPrime | eps */
  NT_TERM,                 /* B3 */
  NT_TERMPRIME,            /* B4 – highPrecOp factor termPrime | eps */
  NT_FACTOR,               /* B5 – TK_OP arith TK_CL | var */
  NT_HIGHPRECEDENCEOP,     /* B6 – TK_MUL | TK_DIV */
  NT_LOWPRECEDENCEOP,      /* B7 – TK_PLUS | TK_MINUS */
  NT_BOOLEANEXPRESSION,
  NT_VAR, /* 39-a – singleOrRecId | TK_NUM | TK_RNUM */
  NT_LOGICALOP,
  NT_RELATIONALOP,
  NT_RETURNSTMT,
  NT_OPTIONALRETURN,
  NT_IDLIST,
  NT_MORE_IDS,
  NT_DEFINETYPESTMT,
  NT_A,
  NT_COUNT /* sentinel – total non-terminals */
} NonTerminal;

/* -----------------------------------------------
 * Grammar symbol
 * ----------------------------------------------- */
typedef enum { SYMBOL_TERMINAL, SYMBOL_NON_TERMINAL } SymbolType;

typedef struct {
  SymbolType type;
  union {
    TokenType terminal;
    NonTerminal nonTerminal;
  } symbol;
} GrammarSymbol;

/* -----------------------------------------------
 * Grammar rule
 * ----------------------------------------------- */
typedef struct {
  NonTerminal lhs;
  GrammarSymbol rhs[MAX_RHS];
  int rhsCount;
  int ruleNumber;
} GrammarRule;

typedef struct {
  GrammarRule rules[MAX_RULES];
  int ruleCount;
  NonTerminal startSymbol;
} grammar;

/* -----------------------------------------------
 * FIRST / FOLLOW sets
 * ----------------------------------------------- */
typedef struct {
  NonTerminal nonTerminal;
  TokenType first[MAX_FIRST_FOLLOW];
  int firstCount;
  int hasEpsilon;
} FirstSet;

typedef struct {
  NonTerminal nonTerminal;
  TokenType follow[MAX_FIRST_FOLLOW];
  int followCount;
} FollowSet;

typedef struct {
  FirstSet firstSets[MAX_NT];
  int firstCount;
  FollowSet followSets[MAX_NT];
  int followCount;
} FirstAndFollow;

/* -----------------------------------------------
 * Parse table
 * ----------------------------------------------- */
typedef struct {
  int isValid;
  int ruleNumber;
  GrammarRule *rule;
} ParseTableEntry;

typedef struct {
  ParseTableEntry entries[MAX_NT][TABLE_TERMINALS];
} table;

/* -----------------------------------------------
 * Parse tree
 * ----------------------------------------------- */
typedef struct parseTreeNode {
  GrammarSymbol symbol;
  tokenInfo token; /* valid for leaf nodes */
  struct parseTreeNode *children[MAX_CHILDREN];
  int childCount;
  struct parseTreeNode *parent;
  int isLeaf;
  int visited;
} parseTreeNode;

typedef struct {
  parseTreeNode *root;
} parseTree;

/* -----------------------------------------------
 * Parsing stack
 * ----------------------------------------------- */
typedef struct stackNode {
  GrammarSymbol symbol;
  parseTreeNode *treeNode;
  struct stackNode *next;
} stackNode;

#endif /* PARSERDEF_H */
