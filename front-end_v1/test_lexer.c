#include "lexer.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return 1;
    }
    
    FILE *fp = fopen(argv[1], "r");
    if (!fp) {
        fprintf(stderr, "Cannot open file\n");
        return 1;
    }
    
    twinBuffer *tb = initializeTwinBuffer(fp);
    
    printf("%-20s %-30s %-8s\n", "TOKEN", "LEXEME", "LINE");
    printf("========================================================\n");
    
    tokenInfo token;
    int count = 0;
    do {
        token = getNextToken(tb);
        if (token.tokenType != TK_EOF && token.tokenType != TK_ERROR) {
            printf("%-20s %-30s %-8d\n", 
                   getTokenName(token.tokenType),
                   token.lexeme,
                   token.lineNumber);
            count++;
        } else if (token.tokenType == TK_ERROR) {
            fprintf(stderr, "Error at line %d: %s\n", token.lineNumber, token.lexeme);
        }
    } while (token.tokenType != TK_EOF);
    
    printf("========================================================\n");
    printf("Total tokens: %d\n", count);
    
    freeTwinBuffer(tb);
    return 0;
}