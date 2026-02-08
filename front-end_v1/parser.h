/**
 * parser.h
 * Function prototypes for syntax analyzer
 * CS F363 Compiler Design Project
 */

#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"
#include "lexer.h"

/**
 * Initialize grammar with all production rules
 * @return Initialized grammar structure
 */
grammar* initializeGrammar();

/**
 * Compute FIRST and FOLLOW sets for grammar
 * @param G Grammar structure
 * @return FirstAndFollow structure
 */
FirstAndFollow computeFirstAndFollowSets(grammar *G);

/**
 * Create predictive parsing table from FIRST and FOLLOW sets
 * @param F First and Follow information
 * @param T Parse table to populate
 * @param G Grammar structure
 */
void createParseTable(FirstAndFollow *F, table *T, grammar *G);

/**
 * Parse input source code and create parse tree
 * @param testcaseFile Source code file
 * @param T Parse table
 * @param G Grammar structure
 * @return Parse tree if successful, NULL otherwise
 */
parseTree* parseInputSourceCode(char *testcaseFile, table *T, grammar *G);

/**
 * Print parse tree to file in specified format
 * @param PT Parse tree
 * @param outfile Output filename
 */
void printParseTree(parseTree *PT, char *outfile);

/**
 * Print parse table for verification
 * @param T Parse table
 * @param G Grammar structure
 */
void printParseTable(table *T, grammar *G);

/**
 * Print FIRST and FOLLOW sets
 * @param F FirstAndFollow structure
 */
void printFirstAndFollow(FirstAndFollow *F);

/**
 * Get non-terminal name as string
 * @param nt Non-terminal
 * @return String representation
 */
const char* getNonTerminalName(NonTerminal nt);

/**
 * Create parse tree node
 * @param symbol Grammar symbol
 * @return New parse tree node
 */
parseTreeNode* createTreeNode(GrammarSymbol symbol);

/**
 * Add child to parse tree node
 * @param parent Parent node
 * @param child Child node
 */
void addChild(parseTreeNode *parent, parseTreeNode *child);

/**
 * Free parse tree memory
 * @param PT Parse tree
 */
void freeParseTree(parseTree *PT);

/**
 * Stack operations for predictive parsing
 */
stackNode* createStackNode(GrammarSymbol symbol, parseTreeNode *treeNode);
void push(stackNode **top, GrammarSymbol symbol, parseTreeNode *treeNode);
stackNode* pop(stackNode **top);
int isStackEmpty(stackNode *top);
GrammarSymbol topSymbol(stackNode *top);

/**
 * Helper functions for FIRST and FOLLOW computation
 */
void computeFirst(NonTerminal nt, grammar *G, FirstAndFollow *F);
void computeFollow(NonTerminal nt, grammar *G, FirstAndFollow *F);
int hasEpsilon(NonTerminal nt, FirstAndFollow *F);
void addToFirst(FirstSet *set, TokenType token);
void addToFollow(FollowSet *set, TokenType token);

/**
 * Print inorder traversal of parse tree
 */
void printInorder(parseTreeNode *node, FILE *fp, parseTreeNode *root);

/**
 * Report syntax error
 */
void reportSyntaxError(ParseError *error);

#endif /* PARSER_H */
