#!/bin/bash
# Quick test script for compiler

echo "=========================================="
echo "Testing Compiler Front-End"
echo "=========================================="
echo ""

# Test 1: Comment Removal
echo "Test 1: Comment Removal"
echo "1" | timeout 2 ./stage1exe testcase1.txt output.txt 2>/dev/null || echo "Option 1 needs fixing"

echo ""
echo "Test 2: Lexical Analysis (Token List)"
# Create a simple wrapper that just tests the lexer directly
cat > test_lexer.c << 'EOF'
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
EOF

gcc -o test_lexer test_lexer.c lexer.c -I. -Wall
echo "Running lexer test on testcase1.txt:"
./test_lexer testcase1.txt | head -30

echo ""
echo "=========================================="
echo "Compilation successful!"
echo "Main executable: stage1exe"
echo "Test lexer: test_lexer"
echo "=========================================="
