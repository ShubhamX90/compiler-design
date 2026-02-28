/**
 * parser.h
 * Function prototypes for the LL(1) predictive parser
 * CS F363 Compiler Design Project - BITS Pilani
 */

#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"
#include "lexer.h"

/* Grammar initialization */
grammar*       initializeGrammar(void);

/* FIRST / FOLLOW computation */
FirstAndFollow computeFirstAndFollowSets(grammar *G);
void           printFirstAndFollow(FirstAndFollow *F);

/* Parse table */
void createParseTable(FirstAndFollow *F, table *T, grammar *G);
void printParseTable(table *T, grammar *G);

/* Parsing */
parseTree* parseInputSourceCode(char *testcaseFile, table *T, grammar *G);

/* Parse tree output */
void printParseTree(parseTree *PT, char *outfile);
void freeParseTree(parseTree *PT);

/* Non-terminal name */
const char* getNonTerminalName(NonTerminal nt);

#endif /* PARSER_H */
