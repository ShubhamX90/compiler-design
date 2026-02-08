/**
 * driver.c
 * Main driver program for compiler front-end
 * CS F363 Compiler Design Project
 * 
 * Provides menu-driven interface for:
 * 1. Comment removal
 * 2. Lexical analysis
 * 3. Syntax analysis with parse tree
 * 4. Performance timing
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "lexer.h"
#include "parser.h"

/* Function prototypes */
void printImplementationStatus();
void optionCommentRemoval(char *sourceFile);
void optionPrintTokens(char *sourceFile);
void optionParsing(char *sourceFile, char *parseTreeFile);
void optionTiming(char *sourceFile);
void printMenu();

int main(int argc, char *argv[]) {
    /* Print implementation status */
    printImplementationStatus();
    
    /* Check command line arguments */
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <source_file.txt> <parsetree_output.txt>\n", argv[0]);
        fprintf(stderr, "Example: ./stage1exe testcase.txt parsetreeOutFile.txt\n");
        return 1;
    }
    
    char *sourceFile = argv[1];
    char *parseTreeFile = argv[2];
    
    /* Verify source file exists */
    FILE *testFile = fopen(sourceFile, "r");
    if (!testFile) {
        fprintf(stderr, "Error: Cannot open source file %s\n", sourceFile);
        return 1;
    }
    fclose(testFile);
    
    /* Main menu loop */
    int option;
    printf("\n========================================\n");
    printf("  COMPILER FRONT-END - CS F363\n");
    printf("========================================\n");
    
    do {
        printMenu();
        printf("Enter option: ");
        
        if (scanf("%d", &option) != 1) {
            /* Clear input buffer */
            while (getchar() != '\n');
            printf("Invalid input. Please enter a number.\n");
            continue;
        }
        
        printf("\n");
        
        switch (option) {
            case 0:
                printf("Exiting compiler...\n");
                break;
                
            case 1:
                printf("========== COMMENT REMOVAL ==========\n");
                optionCommentRemoval(sourceFile);
                break;
                
            case 2:
                printf("========== LEXICAL ANALYSIS ==========\n");
                optionPrintTokens(sourceFile);
                break;
                
            case 3:
                printf("========== SYNTAX ANALYSIS ==========\n");
                optionParsing(sourceFile, parseTreeFile);
                break;
                
            case 4:
                printf("========== PERFORMANCE TIMING ==========\n");
                optionTiming(sourceFile);
                break;
                
            default:
                printf("Invalid option. Please choose 0-4.\n");
                break;
        }
        
        if (option != 0) {
            printf("\nPress Enter to continue...");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);  /* Clear buffer */
        }
        
    } while (option != 0);
    
    printf("\nThank you for using our compiler!\n");
    return 0;
}

/**
 * Print implementation status
 */
void printImplementationStatus() {
    printf("\n========== IMPLEMENTATION STATUS ==========\n");
    printf("(a) FIRST and FOLLOW set automated: YES\n");
    printf("(b) Lexical analyzer module developed: YES\n");
    printf("(c) Both lexical and syntax analysis modules implemented: YES\n");
    printf("(d) Modules compile: YES\n");
    printf("(e) Modules work with all testcases: YES\n");
    printf("(f) Parse tree constructed: YES\n");
    printf("===========================================\n");
}

/**
 * Print menu options
 */
void printMenu() {
    printf("\n========== MENU ==========\n");
    printf("0 : Exit\n");
    printf("1 : Remove comments and print clean code\n");
    printf("2 : Print token list (lexical analysis)\n");
    printf("3 : Parse and verify syntax (with parse tree)\n");
    printf("4 : Print total time taken\n");
    printf("==========================\n");
}

/**
 * Option 1: Remove comments from source code
 */
void optionCommentRemoval(char *sourceFile) {
    char cleanFile[] = "clean_code.txt";
    removeComments(sourceFile, cleanFile);
    
    /* Print clean code to console */
    FILE *fp = fopen(cleanFile, "r");
    if (fp) {
        printf("\n---------- CLEAN CODE ----------\n");
        char ch;
        while ((ch = fgetc(fp)) != EOF) {
            printf("%c", ch);
        }
        printf("\n--------------------------------\n");
        fclose(fp);
    }
}

/**
 * Option 2: Print all tokens from lexical analysis
 */
void optionPrintTokens(char *sourceFile) {
    FILE *fp = fopen(sourceFile, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file %s\n", sourceFile);
        return;
    }
    
    twinBuffer *tb = initializeTwinBuffer(fp);
    
    /* Print header */
    printf("\n%-20s %-30s %-8s %-15s\n", "TOKEN_NAME", "LEXEME", "LINE", "VALUE");
    printf("==========================================================================\n");
    
    /* Get and print all tokens */
    tokenInfo token;
    int tokenCount = 0;
    int errorCount = 0;
    
    do {
        token = getNextToken(tb);
        
        if (token.tokenType == TK_ERROR) {
            errorCount++;
            fprintf(stderr, "Line %d: Lexical Error - '%s'\n", 
                    token.lineNumber, token.lexeme);
        } else if (token.tokenType != TK_EOF) {
            /* Print token info */
            printf("%-20s %-30s %-8d ", 
                   getTokenName(token.tokenType), 
                   token.lexeme, 
                   token.lineNumber);
            
            if (token.hasValue) {
                if (token.tokenType == TK_NUM) {
                    printf("%-15d", token.value.intValue);
                } else if (token.tokenType == TK_RNUM) {
                    printf("%-15.2f", token.value.realValue);
                }
            } else {
                printf("%-15s", "----");
            }
            
            printf("\n");
            tokenCount++;
        }
        
    } while (token.tokenType != TK_EOF);
    
    printf("==========================================================================\n");
    printf("Total tokens: %d\n", tokenCount);
    if (errorCount > 0) {
        printf("Total lexical errors: %d\n", errorCount);
    }
    
    freeTwinBuffer(tb);
}

/**
 * Option 3: Parse source code and create parse tree
 */
void optionParsing(char *sourceFile, char *parseTreeFile) {
    /* Initialize grammar */
    printf("Initializing grammar...\n");
    grammar *G = initializeGrammar();
    printf("Grammar initialized with %d rules\n", G->ruleCount);
    
    /* Compute FIRST and FOLLOW sets */
    printf("Computing FIRST and FOLLOW sets...\n");
    FirstAndFollow F = computeFirstAndFollowSets(G);
    printf("FIRST and FOLLOW sets computed\n");
    
    /* Create parse table */
    printf("Creating predictive parse table...\n");
    table T;
    createParseTable(&F, &T, G);
    printf("Parse table created\n");
    
    /* Parse input */
    printf("\nParsing source code...\n");
    parseTree *PT = parseInputSourceCode(sourceFile, &T, G);
    
    if (PT && PT->root) {
        /* Print parse tree */
        printParseTree(PT, parseTreeFile);
        
        /* Free memory */
        freeParseTree(PT);
    } else {
        fprintf(stderr, "Parsing failed - no parse tree generated\n");
    }
    
    free(G);
}

/**
 * Option 4: Measure and print execution time
 */
void optionTiming(char *sourceFile) {
    clock_t start_time, end_time;
    double total_CPU_time, total_CPU_time_in_seconds;
    
    start_time = clock();
    
    /* Initialize grammar */
    grammar *G = initializeGrammar();
    
    /* Compute FIRST and FOLLOW sets */
    FirstAndFollow F = computeFirstAndFollowSets(G);
    
    /* Create parse table */
    table T;
    createParseTable(&F, &T, G);
    
    /* Parse input */
    parseTree *PT = parseInputSourceCode(sourceFile, &T, G);
    
    end_time = clock();
    
    /* Calculate time */
    total_CPU_time = (double)(end_time - start_time);
    total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
    
    printf("\n========== PERFORMANCE METRICS ==========\n");
    printf("Total CPU time: %.0f clock ticks\n", total_CPU_time);
    printf("Total CPU time in seconds: %.6f seconds\n", total_CPU_time_in_seconds);
    printf("=========================================\n");
    
    /* Free memory */
    if (PT) {
        freeParseTree(PT);
    }
    free(G);
}
