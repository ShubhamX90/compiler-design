/**
 * lexer.c
 * Lexical Analyzer Implementation
 * CS F363 Compiler Design Project - BITS Pilani
 *
 * DFA-based tokenization with twin buffer for efficient I/O.
 * Handles all tokens per Table 1 in the language specification.
 *
 * Key fixes/features:
 *  - Twin buffer (4096 bytes each half) for O(1) character access
 *  - Proper retraction for <-  (double retract → TK_LT + TK_MINUS)
 *  - 23.abc → "23." as TK_ERROR, "abc" as TK_FIELDID  (Announcement 2)
 *  - TK_COMMENT returned (not silently skipped) so option 2 can display them
 *  - Descriptive error messages on TK_ERROR tokens
 */

#include "lexerDef.h"

/* -----------------------------------------------
 * Keyword lookup table
 * ----------------------------------------------- */
static KeywordEntry keywordTable[] = {{"with", TK_WITH},
                                      {"parameters", TK_PARAMETERS},
                                      {"end", TK_END},
                                      {"while", TK_WHILE},
                                      {"union", TK_UNION},
                                      {"endunion", TK_ENDUNION},
                                      {"definetype", TK_DEFINETYPE},
                                      {"as", TK_AS},
                                      {"type", TK_TYPE},
                                      {"global", TK_GLOBAL},
                                      {"parameter", TK_PARAMETER},
                                      {"list", TK_LIST},
                                      {"input", TK_INPUT},
                                      {"output", TK_OUTPUT},
                                      {"int", TK_INT},
                                      {"real", TK_REAL},
                                      {"endwhile", TK_ENDWHILE},
                                      {"if", TK_IF},
                                      {"then", TK_THEN},
                                      {"endif", TK_ENDIF},
                                      {"read", TK_READ},
                                      {"write", TK_WRITE},
                                      {"return", TK_RETURN},
                                      {"call", TK_CALL},
                                      {"record", TK_RECORD},
                                      {"endrecord", TK_ENDRECORD},
                                      {"else", TK_ELSE},
                                      {NULL, TK_ERROR}};

/* -----------------------------------------------
 * Character class helpers
 * ----------------------------------------------- */
static inline int isDIG(char c) { return c >= '0' && c <= '9'; }
static inline int isD27(char c) { return c >= '2' && c <= '7'; }
static inline int isLOW(char c) { return c >= 'a' && c <= 'z'; }
static inline int isBD(char c) { return c == 'b' || c == 'c' || c == 'd'; }
static inline int isLET(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
static inline int isWS(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

/* -----------------------------------------------
 * Twin buffer management
 * ----------------------------------------------- */
twinBuffer *initializeTwinBuffer(FILE *fp) {
  twinBuffer *tb = (twinBuffer *)malloc(sizeof(twinBuffer));
  if (!tb) {
    fprintf(stderr, "Error: malloc failed for twin buffer\n");
    exit(1);
  }

  tb->fp = fp;
  tb->forward = 0;
  tb->lexemeBegin = 0;
  tb->eof1 = 0;
  tb->eof2 = 0;
  tb->lineNumber = 1;
  memset(tb->buffer, 0, sizeof(tb->buffer));

  /* Load first half */
  size_t n = fread(tb->buffer, 1, BUFFER_SIZE, fp);
  if (n < BUFFER_SIZE) {
    tb->buffer[n] = (char)EOF;
    tb->eof1 = 1;
  }

  /* Load second half */
  n = fread(tb->buffer + BUFFER_SIZE, 1, BUFFER_SIZE, fp);
  if (n < BUFFER_SIZE) {
    tb->buffer[BUFFER_SIZE + n] = (char)EOF;
    tb->eof2 = 1;
  }

  return tb;
}

char getNextChar(twinBuffer *tb) {
  char c = tb->buffer[tb->forward];

  /* EOF sentinel hit */
  if ((unsigned char)c == (unsigned char)EOF) {
    if ((tb->forward < BUFFER_SIZE && tb->eof1) ||
        (tb->forward >= BUFFER_SIZE && tb->eof2))
      return (char)EOF;
  }

  tb->forward++;

  /* Reload first half when moving into second */
  if (tb->forward == BUFFER_SIZE && !tb->eof1) {
    size_t n = fread(tb->buffer, 1, BUFFER_SIZE, tb->fp);
    if (n < BUFFER_SIZE) {
      tb->buffer[n] = (char)EOF;
      tb->eof1 = 1;
    }
  }
  /* Reload second half and wrap around */
  else if (tb->forward == TWIN_BUFFER_SIZE) {
    if (!tb->eof2) {
      size_t n = fread(tb->buffer + BUFFER_SIZE, 1, BUFFER_SIZE, tb->fp);
      if (n < BUFFER_SIZE) {
        tb->buffer[BUFFER_SIZE + n] = (char)EOF;
        tb->eof2 = 1;
      }
    }
    tb->forward = 0;
  }

  if (c == '\n')
    tb->lineNumber++;
  return c;
}

void retract(twinBuffer *tb, int n) {
  for (int i = 0; i < n; i++) {
    if (tb->forward > 0)
      tb->forward--;
    else
      tb->forward = TWIN_BUFFER_SIZE - 1;
    if (tb->buffer[tb->forward] == '\n')
      tb->lineNumber--;
  }
}

void getLexeme(twinBuffer *tb, char *lexeme) {
  int i = 0;
  int pos = tb->lexemeBegin;
  while (pos != tb->forward && i < MAX_LEXEME_LENGTH - 1) {
    lexeme[i++] = tb->buffer[pos];
    if (++pos == TWIN_BUFFER_SIZE)
      pos = 0;
  }
  lexeme[i] = '\0';
}

void freeTwinBuffer(twinBuffer *tb) {
  if (tb) {
    if (tb->fp)
      fclose(tb->fp);
    free(tb);
  }
}

/* -----------------------------------------------
 * Keyword lookup
 * ----------------------------------------------- */
TokenType lookupKeyword(const char *lexeme) {
  for (int i = 0; keywordTable[i].keyword != NULL; i++)
    if (strcmp(lexeme, keywordTable[i].keyword) == 0)
      return keywordTable[i].token;
  return TK_FIELDID;
}

/* -----------------------------------------------
 * Main tokenizer  (DFA-based)
 * ----------------------------------------------- */
tokenInfo getNextToken(twinBuffer *tb) {
  tokenInfo token;
  memset(&token, 0, sizeof(token));
  char c;

restart:
  tb->lexemeBegin = tb->forward;
  int startLine = tb->lineNumber;
  c = getNextChar(tb);

  /* EOF */
  if ((unsigned char)c == (unsigned char)EOF) {
    token.tokenType = TK_EOF;
    token.lineNumber = startLine;
    strcpy(token.lexeme, "EOF");
    return token;
  }

  /* ---- Whitespace: skip and restart ---- */
  if (isWS(c)) {
    while (isWS(c))
      c = getNextChar(tb);
    /* Only retract if we stopped on a real character.
     * getNextChar does NOT advance forward when it returns EOF,
     * so retracting would move backward to the last whitespace
     * and cause an infinite restart cycle. */
    if ((unsigned char)c != (unsigned char)EOF)
      retract(tb, 1);
    goto restart;
  }

  /* ---- Comment: read to end of line, return TK_COMMENT ---- */
  if (c == '%') {
    while (c != '\n' && (unsigned char)c != (unsigned char)EOF)
      c = getNextChar(tb);
    if (c == '\n')
      retract(tb, 1); /* keep newline for line tracking */
    token.tokenType = TK_COMMENT;
    token.lineNumber = startLine;
    strcpy(token.lexeme, "%");
    return token;
  }

  /* ---- Digits: integer or real ---- */
  if (isDIG(c)) {
    /* consume all leading digits */
    while (isDIG(c = getNextChar(tb)))
      ;
    if ((unsigned char)c == (unsigned char)EOF || !isDIG(c)) {
      if (c == '.') {
        /* Dot after integer: need exactly 2 fraction digits */
        char c2 = getNextChar(tb);
        if (isDIG(c2)) {
          char c3 = getNextChar(tb);
          if (isDIG(c3)) {
            /* Could be TK_RNUM or exponent form */
            char c4 = getNextChar(tb);
            if (c4 == 'E') {
              /* Exponent: +/- or digit digit */
              char c5 = getNextChar(tb);
              int hasSign = (c5 == '+' || c5 == '-');
              if (hasSign)
                c5 = getNextChar(tb);
              if (isDIG(c5)) {
                char c6 = getNextChar(tb);
                if (isDIG(c6)) {
                  /* Valid: NNN.NNE[+-]NN */
                  retract(tb, 1);
                  getLexeme(tb, token.lexeme);
                  token.tokenType = TK_RNUM;
                  token.value.realValue = atof(token.lexeme);
                  token.hasValue = 1;
                  token.lineNumber = startLine;
                  return token;
                } else {
                  /* Only 1 exponent digit */
                  retract(tb, 1);
                  getLexeme(tb, token.lexeme);
                  token.tokenType = TK_ERROR;
                  token.errorType = ERR_UNKNOWN_PATTERN;
                  snprintf(token.errorMsg, sizeof(token.errorMsg),
                           "Unknown pattern <%s>", token.lexeme);
                  token.lineNumber = startLine;
                  return token;
                }
              } else {
                /* No digit after sign/E */
                if (hasSign)
                  retract(tb, 2);
                else
                  retract(tb, 1);
                /* Emit NNN.NN as RNUM (no E part) */
                getLexeme(tb, token.lexeme);
                token.tokenType = TK_RNUM;
                token.value.realValue = atof(token.lexeme);
                token.hasValue = 1;
                token.lineNumber = startLine;
                return token;
              }
            } else {
              /* No E: valid TK_RNUM NNN.NN */
              retract(tb, 1);
              getLexeme(tb, token.lexeme);
              token.tokenType = TK_RNUM;
              token.value.realValue = atof(token.lexeme);
              token.hasValue = 1;
              token.lineNumber = startLine;
              return token;
            }
          } else {
            /* Only 1 fraction digit: NNN.N → error */
            retract(tb, 1);
            getLexeme(tb, token.lexeme);
            token.tokenType = TK_ERROR;
            token.errorType = ERR_UNKNOWN_PATTERN;
            snprintf(token.errorMsg, sizeof(token.errorMsg),
                     "Unknown pattern <%s>", token.lexeme);
            token.lineNumber = startLine;
            return token;
          }
        } else {
          /* NNN.X where X is not digit: "NNN." → TK_ERROR, retract X */
          retract(tb, 1);
          getLexeme(tb, token.lexeme);
          token.tokenType = TK_ERROR;
          token.errorType = ERR_UNKNOWN_PATTERN;
          snprintf(token.errorMsg, sizeof(token.errorMsg),
                   "Unknown pattern <%s>", token.lexeme);
          token.lineNumber = startLine;
          return token;
        }
      } else {
        /* Plain integer */
        if ((unsigned char)c != (unsigned char)EOF)
          retract(tb, 1);
        getLexeme(tb, token.lexeme);
        token.tokenType = TK_NUM;
        token.value.intValue = atoi(token.lexeme);
        token.hasValue = 1;
        token.lineNumber = startLine;
        return token;
      }
    }
    /* fallthrough impossible */
  }

  /* ---- Underscore: _main or TK_FUNID ---- */
  if (c == '_') {
    c = getNextChar(tb);
    if (!isLET(c)) {
      retract(tb, 1);
      token.tokenType = TK_ERROR;
      token.errorType = ERR_UNKNOWN_SYMBOL;
      snprintf(token.errorMsg, sizeof(token.errorMsg), "Unknown Symbol <_>");
      strcpy(token.lexeme, "_");
      token.lineNumber = startLine;
      return token;
    }
    while (isLET(c))
      c = getNextChar(tb);
    while (isDIG(c))
      c = getNextChar(tb);
    if ((unsigned char)c != (unsigned char)EOF)
      retract(tb, 1);
    getLexeme(tb, token.lexeme);
    if (strcmp(token.lexeme, "_main") == 0) {
      token.tokenType = TK_MAIN;
    } else if ((int)strlen(token.lexeme) <= 30) {
      token.tokenType = TK_FUNID;
    } else {
      token.tokenType = TK_ERROR;
      token.errorType = ERR_FUNID_TOO_LONG;
      snprintf(token.errorMsg, sizeof(token.errorMsg),
               "Function identifier exceeds max length of 30 characters.");
    }
    token.lineNumber = startLine;
    return token;
  }

  /* ---- # : record/union identifier ---- */
  if (c == '#') {
    c = getNextChar(tb);
    if (!isLOW(c)) {
      retract(tb, 1);
      token.tokenType = TK_ERROR;
      token.errorType = ERR_UNKNOWN_SYMBOL;
      snprintf(token.errorMsg, sizeof(token.errorMsg), "Unknown Symbol <#>");
      strcpy(token.lexeme, "#");
      token.lineNumber = startLine;
      return token;
    }
    while (isLOW(c))
      c = getNextChar(tb);
    if ((unsigned char)c != (unsigned char)EOF)
      retract(tb, 1);
    getLexeme(tb, token.lexeme);
    token.tokenType = TK_RUID;
    token.lineNumber = startLine;
    return token;
  }

  /* ---- b/c/d : variable ID or keyword ---- */
  if (isBD(c)) {

    c = getNextChar(tb);
    if (isD27(c)) {
      /* Potential TK_ID: [b-d][2-7][b-d]*[2-7]*
       * BUG FIX: c currently holds the mandatory first [2-7] digit.
       * We must advance past it before consuming the optional [b-d]* and [2-7]*
       * tails; otherwise the while-loops both start on the same [2-7] digit,
       * the isBD loop silently skips (digit ≠ b/c/d), the isD27 loop
       * only reads one char and stops on the first [b-d] body letter,
       * causing identifiers like d5cc34 to be cut at d5cc → TK_FIELDID.
       */
      c = getNextChar(tb); /* advance past first [2-7] */
      while (isBD(c))
        c = getNextChar(tb); /* consume [b-d]* */
      while (isD27(c))
        c = getNextChar(tb); /* consume [2-7]* */
      if (!isLOW(c) && !isDIG(c)) {
        if ((unsigned char)c != (unsigned char)EOF)
          retract(tb, 1);
        getLexeme(tb, token.lexeme);
        int len = (int)strlen(token.lexeme);
        if (len >= 2 && len <= 20) {
          token.tokenType = TK_ID;
        } else {
          token.tokenType = TK_ERROR;
          token.errorType = ERR_ID_TOO_LONG;
          snprintf(token.errorMsg, sizeof(token.errorMsg),
                   "Variable Identifier is longer than the prescribed length "
                   "of 20 characters.");
        }
        token.lineNumber = startLine;
        return token;
      } else {
        /* Continuation chars not in pattern → treat as field/keyword */
        while (isLOW(c))
          c = getNextChar(tb);
        if ((unsigned char)c != (unsigned char)EOF)
          retract(tb, 1);
        getLexeme(tb, token.lexeme);
        token.tokenType = lookupKeyword(token.lexeme);
        token.lineNumber = startLine;
        return token;
      }
    } else if (isLOW(c)) {
      /* Part of a longer lowercase word (keyword or fieldid) */
      while (isLOW(c))
        c = getNextChar(tb);
      if ((unsigned char)c != (unsigned char)EOF)
        retract(tb, 1);
      getLexeme(tb, token.lexeme);
      token.tokenType = lookupKeyword(token.lexeme);
      token.lineNumber = startLine;
      return token;
    } else {
      /* Single b/c/d: could be keyword prefix — try as word */
      retract(tb, 1);
      getLexeme(tb, token.lexeme);
      token.tokenType = lookupKeyword(token.lexeme);
      token.lineNumber = startLine;
      return token;
    }
  }

  /* ---- Other lowercase: keyword or TK_FIELDID ---- */
  if (isLOW(c)) {
    while (isLOW(c))
      c = getNextChar(tb);
    if ((unsigned char)c != (unsigned char)EOF)
      retract(tb, 1);
    getLexeme(tb, token.lexeme);
    token.tokenType = lookupKeyword(token.lexeme);
    token.lineNumber = startLine;
    return token;
  }

  /* ---- < ---- */
  if (c == '<') {
    c = getNextChar(tb);
    if (c == '=') {
      token.tokenType = TK_LE;
      strcpy(token.lexeme, "<=");
      token.lineNumber = startLine;
      return token;
    } else if (c == '-') {
      /* <-  */
      c = getNextChar(tb);
      if (c == '-') {
        /* <--  */
        c = getNextChar(tb);
        if (c == '-') {
          /* <--- valid assignment operator */
          token.tokenType = TK_ASSIGNOP;
          strcpy(token.lexeme, "<---");
          token.lineNumber = startLine;
          return token;
        } else {
          /* <-- invalid */
          retract(tb, 1);
          getLexeme(tb, token.lexeme);
          token.tokenType = TK_ERROR;
          token.errorType = ERR_UNKNOWN_PATTERN;
          snprintf(token.errorMsg, sizeof(token.errorMsg),
                   "Unknown pattern <%s>", token.lexeme);
          token.lineNumber = startLine;
          return token;
        }
      } else {
        /* <- : double retract → return TK_LT for '<', next call gets '-' */
        retract(tb, 2);
        token.tokenType = TK_LT;
        strcpy(token.lexeme, "<");
        token.lineNumber = startLine;
        return token;
      }
    } else {
      retract(tb, 1);
      token.tokenType = TK_LT;
      strcpy(token.lexeme, "<");
      token.lineNumber = startLine;
      return token;
    }
  }

  /* ---- > ---- */
  if (c == '>') {
    c = getNextChar(tb);
    if (c == '=') {
      token.tokenType = TK_GE;
      strcpy(token.lexeme, ">=");
    } else {
      retract(tb, 1);
      token.tokenType = TK_GT;
      strcpy(token.lexeme, ">");
    }
    token.lineNumber = startLine;
    return token;
  }

  /* ---- = ---- */
  if (c == '=') {
    c = getNextChar(tb);
    if (c == '=') {
      token.tokenType = TK_EQ;
      strcpy(token.lexeme, "==");
    } else {
      retract(tb, 1);
      token.tokenType = TK_ERROR;
      token.errorType = ERR_UNKNOWN_SYMBOL;
      snprintf(token.errorMsg, sizeof(token.errorMsg), "Unknown Symbol <=>");
      strcpy(token.lexeme, "=");
    }
    token.lineNumber = startLine;
    return token;
  }

  /* ---- ! ---- */
  if (c == '!') {
    c = getNextChar(tb);
    if (c == '=') {
      token.tokenType = TK_NE;
      strcpy(token.lexeme, "!=");
    } else {
      retract(tb, 1);
      token.tokenType = TK_ERROR;
      token.errorType = ERR_UNKNOWN_SYMBOL;
      snprintf(token.errorMsg, sizeof(token.errorMsg), "Unknown Symbol <!>");
      strcpy(token.lexeme, "!");
    }
    token.lineNumber = startLine;
    return token;
  }

  /* ---- & : &&& ---- */
  if (c == '&') {
    c = getNextChar(tb);
    if (c == '&') {
      c = getNextChar(tb);
      if (c == '&') {
        token.tokenType = TK_AND;
        strcpy(token.lexeme, "&&&");
      } else {
        retract(tb, 1);
        token.tokenType = TK_ERROR;
        token.errorType = ERR_UNKNOWN_PATTERN;
        snprintf(token.errorMsg, sizeof(token.errorMsg),
                 "Unknown pattern <&&>");
        strcpy(token.lexeme, "&&");
      }
    } else {
      retract(tb, 1);
      token.tokenType = TK_ERROR;
      token.errorType = ERR_UNKNOWN_SYMBOL;
      snprintf(token.errorMsg, sizeof(token.errorMsg), "Unknown Symbol <&>");
      strcpy(token.lexeme, "&");
    }
    token.lineNumber = startLine;
    return token;
  }

  /* ---- @ : @@@ ---- */
  if (c == '@') {
    c = getNextChar(tb);
    if (c == '@') {
      c = getNextChar(tb);
      if (c == '@') {
        token.tokenType = TK_OR;
        strcpy(token.lexeme, "@@@");
      } else {
        retract(tb, 1);
        token.tokenType = TK_ERROR;
        token.errorType = ERR_UNKNOWN_PATTERN;
        snprintf(token.errorMsg, sizeof(token.errorMsg),
                 "Unknown pattern <@@>");
        strcpy(token.lexeme, "@@");
      }
    } else {
      retract(tb, 1);
      token.tokenType = TK_ERROR;
      token.errorType = ERR_UNKNOWN_SYMBOL;
      snprintf(token.errorMsg, sizeof(token.errorMsg), "Unknown Symbol <@>");
      strcpy(token.lexeme, "@");
    }
    token.lineNumber = startLine;
    return token;
  }

  /* ---- | : invalid ---- */
  if (c == '|') {
    token.tokenType = TK_ERROR;
    token.errorType = ERR_UNKNOWN_SYMBOL;
    snprintf(token.errorMsg, sizeof(token.errorMsg), "Unknown Symbol <|>");
    strcpy(token.lexeme, "|");
    token.lineNumber = startLine;
    return token;
  }

  /* ---- Single-character tokens ---- */
  switch (c) {
  case '~':
    token.tokenType = TK_NOT;
    strcpy(token.lexeme, "~");
    break;
  case '+':
    token.tokenType = TK_PLUS;
    strcpy(token.lexeme, "+");
    break;
  case '-':
    token.tokenType = TK_MINUS;
    strcpy(token.lexeme, "-");
    break;
  case '*':
    token.tokenType = TK_MUL;
    strcpy(token.lexeme, "*");
    break;
  case '/':
    token.tokenType = TK_DIV;
    strcpy(token.lexeme, "/");
    break;
  case ',':
    token.tokenType = TK_COMMA;
    strcpy(token.lexeme, ",");
    break;
  case ';':
    token.tokenType = TK_SEM;
    strcpy(token.lexeme, ";");
    break;
  case ':':
    token.tokenType = TK_COLON;
    strcpy(token.lexeme, ":");
    break;
  case '.':
    token.tokenType = TK_DOT;
    strcpy(token.lexeme, ".");
    break;
  case '(':
    token.tokenType = TK_OP;
    strcpy(token.lexeme, "(");
    break;
  case ')':
    token.tokenType = TK_CL;
    strcpy(token.lexeme, ")");
    break;
  case '[':
    token.tokenType = TK_SQL;
    strcpy(token.lexeme, "[");
    break;
  case ']':
    token.tokenType = TK_SQR;
    strcpy(token.lexeme, "]");
    break;
  default:
    token.tokenType = TK_ERROR;
    token.errorType = ERR_UNKNOWN_SYMBOL;
    snprintf(token.errorMsg, sizeof(token.errorMsg), "Unknown Symbol <%c>", c);
    snprintf(token.lexeme, sizeof(token.lexeme), "%c", c);
    break;
  }
  token.lineNumber = startLine;
  return token;
}

/* -----------------------------------------------
 * Remove comments – write clean source to cleanFile.
 * Line structure (indentation, line numbers) preserved.
 * ----------------------------------------------- */
void removeComments(char *testcaseFile, char *cleanFile) {
  FILE *in = fopen(testcaseFile, "r");
  FILE *out = fopen(cleanFile, "w");
  if (!in) {
    fprintf(stderr, "Error: Cannot open %s\n", testcaseFile);
    return;
  }
  if (!out) {
    fprintf(stderr, "Error: Cannot create %s\n", cleanFile);
    fclose(in);
    return;
  }

  int ch, inComment = 0;
  while ((ch = fgetc(in)) != EOF) {
    if (ch == '%')
      inComment = 1;
    if (!inComment)
      fputc(ch, out);
    if (inComment && ch == '\n') {
      inComment = 0;
      fputc('\n', out);
    }
  }
  fclose(in);
  fclose(out);
}

/* -----------------------------------------------
 * Token name string
 * ----------------------------------------------- */
const char *getTokenName(TokenType token) {
  switch (token) {
  case TK_ASSIGNOP:
    return "TK_ASSIGNOP";
  case TK_COMMENT:
    return "TK_COMMENT";
  case TK_FIELDID:
    return "TK_FIELDID";
  case TK_ID:
    return "TK_ID";
  case TK_NUM:
    return "TK_NUM";
  case TK_RNUM:
    return "TK_RNUM";
  case TK_FUNID:
    return "TK_FUNID";
  case TK_RUID:
    return "TK_RUID";
  case TK_WITH:
    return "TK_WITH";
  case TK_PARAMETERS:
    return "TK_PARAMETERS";
  case TK_END:
    return "TK_END";
  case TK_WHILE:
    return "TK_WHILE";
  case TK_UNION:
    return "TK_UNION";
  case TK_ENDUNION:
    return "TK_ENDUNION";
  case TK_DEFINETYPE:
    return "TK_DEFINETYPE";
  case TK_AS:
    return "TK_AS";
  case TK_TYPE:
    return "TK_TYPE";
  case TK_MAIN:
    return "TK_MAIN";
  case TK_GLOBAL:
    return "TK_GLOBAL";
  case TK_PARAMETER:
    return "TK_PARAMETER";
  case TK_LIST:
    return "TK_LIST";
  case TK_SQL:
    return "TK_SQL";
  case TK_SQR:
    return "TK_SQR";
  case TK_INPUT:
    return "TK_INPUT";
  case TK_OUTPUT:
    return "TK_OUTPUT";
  case TK_INT:
    return "TK_INT";
  case TK_REAL:
    return "TK_REAL";
  case TK_COMMA:
    return "TK_COMMA";
  case TK_SEM:
    return "TK_SEM";
  case TK_COLON:
    return "TK_COLON";
  case TK_DOT:
    return "TK_DOT";
  case TK_ENDWHILE:
    return "TK_ENDWHILE";
  case TK_OP:
    return "TK_OP";
  case TK_CL:
    return "TK_CL";
  case TK_IF:
    return "TK_IF";
  case TK_THEN:
    return "TK_THEN";
  case TK_ENDIF:
    return "TK_ENDIF";
  case TK_READ:
    return "TK_READ";
  case TK_WRITE:
    return "TK_WRITE";
  case TK_RETURN:
    return "TK_RETURN";
  case TK_PLUS:
    return "TK_PLUS";
  case TK_MINUS:
    return "TK_MINUS";
  case TK_MUL:
    return "TK_MUL";
  case TK_DIV:
    return "TK_DIV";
  case TK_CALL:
    return "TK_CALL";
  case TK_RECORD:
    return "TK_RECORD";
  case TK_ENDRECORD:
    return "TK_ENDRECORD";
  case TK_ELSE:
    return "TK_ELSE";
  case TK_AND:
    return "TK_AND";
  case TK_OR:
    return "TK_OR";
  case TK_NOT:
    return "TK_NOT";
  case TK_LT:
    return "TK_LT";
  case TK_LE:
    return "TK_LE";
  case TK_EQ:
    return "TK_EQ";
  case TK_GT:
    return "TK_GT";
  case TK_GE:
    return "TK_GE";
  case TK_NE:
    return "TK_NE";
  case TK_EOF:
    return "TK_EOF";
  case TK_ERROR:
    return "TK_ERROR";
  case TK_EPSILON:
    return "TK_EPSILON";
  default:
    return "UNKNOWN";
  }
}
