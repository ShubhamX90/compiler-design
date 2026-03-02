/* Group 10
2022B5A70783P Shiv Tiwari
2022B5A70724P Samyek Jain
2022B3A70551P Ayush Jain
2022B4A71282P Nachiket Garg
2022B5A71326P Mayukh Khetan
2022B5A70763P Shubham Mishra
*/

#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "parserDef.h"

grammar *initializeGrammar(void);

FirstAndFollow computeFirstAndFollowSets(grammar *G);

void printFirstAndFollow(FirstAndFollow *F);

void createParseTable(FirstAndFollow *F, table *T, grammar *G);

void printParseTable(table *T, grammar *G);

parseTree *parseInputSourceCode(char *testcaseFile, table *T, grammar *G);

void printParseTree(parseTree *PT, char *outfile);

void freeParseTree(parseTree *PT);

const char *getNonTerminalName(NonTerminal nt);

#endif
