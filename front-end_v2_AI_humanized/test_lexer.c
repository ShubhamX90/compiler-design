/* test_lexer.c - standalone harness to test the lexer on a source file */

#include "lexer.h"
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
    return 1;
  }

  FILE *fp = fopen(argv[1], "r");
  if (!fp) {
    fprintf(stderr, "Error: Cannot open file %s\n", argv[1]);
    return 1;
  }

  twinBuffer *tb = initializeTwinBuffer(fp);

  printf("\n%-10s  %-30s  %-22s  %-8s  %-15s\n", "Line No.", "Lexeme", "Token",
         "HasVal", "Value");
  printf("%s\n",
         "-------------------------------------------------------------------"
         "---------------------");

  tokenInfo token;
  int tokenCount = 0, errorCount = 0;

  do {
    token = getNextToken(tb);

    if (token.tokenType == TK_ERROR) {
      fprintf(stderr, "Line %d\tError: %s\n", token.lineNumber,
              token.errorMsg[0] ? token.errorMsg : token.lexeme);
      errorCount++;
      printf("%-10d  %-30s  %-22s  %-8s  %-15s\n", token.lineNumber,
             token.lexeme, "TK_ERROR", "---",
             token.errorMsg[0] ? token.errorMsg : "lexical error");
      tokenCount++;
    } else if (token.tokenType == TK_EOF) {
      break;
    } else {
      char valueStr[32] = "----";
      char hasValStr[8] = "no";
      if (token.hasValue) {
        strcpy(hasValStr, "yes");
        if (token.tokenType == TK_NUM)
          snprintf(valueStr, sizeof(valueStr), "%d", token.value.intValue);
        else if (token.tokenType == TK_RNUM)
          snprintf(valueStr, sizeof(valueStr), "%.4f", token.value.realValue);
      }
      printf("%-10d  %-30s  %-22s  %-8s  %-15s\n", token.lineNumber,
             token.lexeme, getTokenName(token.tokenType), hasValStr, valueStr);
      tokenCount++;
    }
  } while (token.tokenType != TK_EOF);

  printf("%s\n",
         "-------------------------------------------------------------------"
         "---------------------");
  printf("Total tokens listed: %d\n", tokenCount);
  if (errorCount > 0)
    printf("Total lexical errors: %d  (details on stderr)\n", errorCount);

  freeTwinBuffer(tb);
  return 0;
}