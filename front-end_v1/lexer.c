/**
 * lexer.c
 * Lexical Analyzer Implementation
 * CS F363 Compiler Design Project
 * 
 * Implements DFA-based tokenization with twin buffer for efficient I/O
 */

#include "lexerDef.h"
#include "lexer.h"

/* Keyword lookup table - based on language specification */
static KeywordEntry keywordTable[] = {
    {"with", TK_WITH},
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
    {NULL, TK_ERROR}  // Sentinel
};

/* Character class checking functions */
int isDIG(char c) { return c >= '0' && c <= '9'; }
int isD27(char c) { return c >= '2' && c <= '7'; }
int isLOW(char c) { return c >= 'a' && c <= 'z'; }
int isBD(char c) { return c == 'b' || c == 'c' || c == 'd'; }
int isLET(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'); }
int isWS(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }

/**
 * Initialize twin buffer with file pointer
 */
twinBuffer* initializeTwinBuffer(FILE *fp) {
    twinBuffer *tb = (twinBuffer*)malloc(sizeof(twinBuffer));
    if (!tb) {
        fprintf(stderr, "Error: Memory allocation failed for twin buffer\n");
        exit(1);
    }
    
    tb->fp = fp;
    tb->forward = 0;
    tb->lexemeBegin = 0;
    tb->currentBuffer = 0;
    tb->eof1 = 0;
    tb->eof2 = 0;
    tb->lineNumber = 1;
    
    /* Initialize buffer with EOF markers */
    memset(tb->buffer, 0, TWIN_BUFFER_SIZE + 1);
    
    /* Load first buffer */
    size_t bytesRead = fread(tb->buffer, 1, BUFFER_SIZE, fp);
    if (bytesRead < BUFFER_SIZE) {
        tb->buffer[bytesRead] = EOF;
        tb->eof1 = 1;
    }
    
    /* Load second buffer */
    bytesRead = fread(tb->buffer + BUFFER_SIZE, 1, BUFFER_SIZE, fp);
    if (bytesRead < BUFFER_SIZE) {
        tb->buffer[BUFFER_SIZE + bytesRead] = EOF;
        tb->eof2 = 1;
    }
    
    return tb;
}

/**
 * Get next character from twin buffer with automatic reload
 */
char getNextChar(twinBuffer *tb) {
    char c = tb->buffer[tb->forward];
    
    /* Check for EOF */
    if (c == EOF) {
        if ((tb->forward < BUFFER_SIZE && tb->eof1) || 
            (tb->forward >= BUFFER_SIZE && tb->eof2)) {
            return EOF;
        }
    }
    
    /* Move forward pointer */
    tb->forward++;
    
    /* Check for buffer boundary and reload if needed */
    if (tb->forward == BUFFER_SIZE && !tb->eof1) {
        /* Reload first buffer */
        size_t bytesRead = fread(tb->buffer, 1, BUFFER_SIZE, tb->fp);
        if (bytesRead < BUFFER_SIZE) {
            tb->buffer[bytesRead] = EOF;
            tb->eof1 = 1;
        }
    } else if (tb->forward == TWIN_BUFFER_SIZE && !tb->eof2) {
        /* Reload second buffer and wrap around */
        size_t bytesRead = fread(tb->buffer + BUFFER_SIZE, 1, BUFFER_SIZE, tb->fp);
        if (bytesRead < BUFFER_SIZE) {
            tb->buffer[BUFFER_SIZE + bytesRead] = EOF;
            tb->eof2 = 1;
        }
        tb->forward = 0;
    } else if (tb->forward == TWIN_BUFFER_SIZE) {
        /* Wrap around to beginning */
        tb->forward = 0;
    }
    
    /* Track line numbers */
    if (c == '\n') {
        tb->lineNumber++;
    }
    
    return c;
}

/**
 * Retract forward pointer by n positions
 */
void retract(twinBuffer *tb, int n) {
    for (int i = 0; i < n; i++) {
        if (tb->forward > 0) {
            tb->forward--;
            if (tb->buffer[tb->forward] == '\n') {
                tb->lineNumber--;
            }
        } else {
            tb->forward = TWIN_BUFFER_SIZE - 1;
            if (tb->buffer[tb->forward] == '\n') {
                tb->lineNumber--;
            }
        }
    }
}

/**
 * Extract lexeme from twin buffer
 */
void getLexeme(twinBuffer *tb, char *lexeme) {
    int i = 0;
    int pos = tb->lexemeBegin;
    
    while (pos != tb->forward && i < MAX_LEXEME_LENGTH - 1) {
        lexeme[i++] = tb->buffer[pos];
        pos++;
        if (pos == TWIN_BUFFER_SIZE) pos = 0;
    }
    lexeme[i] = '\0';
}

/**
 * Lookup keyword in table
 */
TokenType lookupKeyword(const char *lexeme) {
    for (int i = 0; keywordTable[i].keyword != NULL; i++) {
        if (strcmp(lexeme, keywordTable[i].keyword) == 0) {
            return keywordTable[i].token;
        }
    }
    return TK_FIELDID;  // Not a keyword, it's a field identifier
}

/**
 * Main tokenization function - implements DFA from design document
 */
tokenInfo getNextToken(twinBuffer *tb) {
    tokenInfo token;
    token.hasValue = 0;
    token.lineNumber = tb->lineNumber;
    memset(token.lexeme, 0, MAX_LEXEME_LENGTH);
    
    DFAState state = S0;
    char c;
    int startLine = tb->lineNumber;
    
    while (1) {
        tb->lexemeBegin = tb->forward;
        startLine = tb->lineNumber;
        c = getNextChar(tb);
        
        /* Handle EOF */
        if (c == EOF) {
            token.tokenType = TK_EOF;
            strcpy(token.lexeme, "EOF");
            token.lineNumber = startLine;
            return token;
        }
        
        /* State S0 - Start */
        if (isWS(c)) {
            state = S1;
        } else if (c == '%') {
            state = S2;
        } else if (isDIG(c)) {
            state = S50;
        } else if (c == '_') {
            state = S30;
        } else if (c == '#') {
            state = S40;
        } else if (isBD(c)) {
            state = S20;
        } else if (isLOW(c) && !isBD(c)) {
            state = S11;
        } else if (c == '<') {
            state = S60;
        } else if (c == '>') {
            state = S63;
        } else if (c == '=') {
            state = S64;
        } else if (c == '!') {
            state = S65;
        } else if (c == '&') {
            state = S66;
        } else if (c == '@') {
            state = S68;
        } else if (c == '~') {
            token.tokenType = TK_NOT;
            strcpy(token.lexeme, "~");
            token.lineNumber = startLine;
            return token;
        } else if (c == '+') {
            token.tokenType = TK_PLUS;
            strcpy(token.lexeme, "+");
            token.lineNumber = startLine;
            return token;
        } else if (c == '-') {
            token.tokenType = TK_MINUS;
            strcpy(token.lexeme, "-");
            token.lineNumber = startLine;
            return token;
        } else if (c == '*') {
            token.tokenType = TK_MUL;
            strcpy(token.lexeme, "*");
            token.lineNumber = startLine;
            return token;
        } else if (c == '/') {
            token.tokenType = TK_DIV;
            strcpy(token.lexeme, "/");
            token.lineNumber = startLine;
            return token;
        } else if (c == ',') {
            token.tokenType = TK_COMMA;
            strcpy(token.lexeme, ",");
            token.lineNumber = startLine;
            return token;
        } else if (c == ';') {
            token.tokenType = TK_SEM;
            strcpy(token.lexeme, ";");
            token.lineNumber = startLine;
            return token;
        } else if (c == ':') {
            token.tokenType = TK_COLON;
            strcpy(token.lexeme, ":");
            token.lineNumber = startLine;
            return token;
        } else if (c == '.') {
            token.tokenType = TK_DOT;
            strcpy(token.lexeme, ".");
            token.lineNumber = startLine;
            return token;
        } else if (c == '(') {
            token.tokenType = TK_OP;
            strcpy(token.lexeme, "(");
            token.lineNumber = startLine;
            return token;
        } else if (c == ')') {
            token.tokenType = TK_CL;
            strcpy(token.lexeme, ")");
            token.lineNumber = startLine;
            return token;
        } else if (c == '[') {
            token.tokenType = TK_SQL;
            strcpy(token.lexeme, "[");
            token.lineNumber = startLine;
            return token;
        } else if (c == ']') {
            token.tokenType = TK_SQR;
            strcpy(token.lexeme, "]");
            token.lineNumber = startLine;
            return token;
        } else {
            /* Error: illegal character */
            token.tokenType = TK_ERROR;
            sprintf(token.lexeme, "%c", c);
            token.lineNumber = startLine;
            return token;
        }
        
        /* Process DFA states */
        break;  // Exit initial check, continue with state machine
    }
    
    /* DFA state machine processing */
    while (1) {
        switch (state) {
            case S1:  /* Whitespace */
                c = getNextChar(tb);
                if (isWS(c)) {
                    continue;
                } else {
                    retract(tb, 1);
                    /* Skip whitespace, get next token */
                    return getNextToken(tb);
                }
                
            case S2:  /* Comment */
                c = getNextChar(tb);
                if (c == '\n' || c == EOF) {
                    /* Skip comment, get next token */
                    return getNextToken(tb);
                } else {
                    continue;
                }
                
            case S11:  /* Lowercase word */
                c = getNextChar(tb);
                if (isLOW(c)) {
                    continue;
                } else {
                    retract(tb, 1);
                    getLexeme(tb, token.lexeme);
                    token.tokenType = lookupKeyword(token.lexeme);
                    token.lineNumber = startLine;
                    return token;
                }
                
            case S20:  /* BCD first letter */
                c = getNextChar(tb);
                if (isD27(c)) {
                    state = S21;
                } else if (isLOW(c)) {
                    state = S11;
                } else {
                    retract(tb, 1);
                    getLexeme(tb, token.lexeme);
                    token.tokenType = lookupKeyword(token.lexeme);
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S21:  /* Var-ID letter zone */
                c = getNextChar(tb);
                if (isBD(c)) {
                    continue;
                } else if (isD27(c)) {
                    state = S22;
                } else {
                    retract(tb, 1);
                    getLexeme(tb, token.lexeme);
                    /* Check length (2-20) */
                    if (strlen(token.lexeme) < 2 || strlen(token.lexeme) > 20) {
                        token.tokenType = TK_ERROR;
                    } else {
                        token.tokenType = TK_ID;
                    }
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S22:  /* Var-ID digit zone */
                c = getNextChar(tb);
                if (isD27(c)) {
                    continue;
                } else {
                    retract(tb, 1);
                    getLexeme(tb, token.lexeme);
                    /* Check length (2-20) */
                    if (strlen(token.lexeme) < 2 || strlen(token.lexeme) > 20) {
                        token.tokenType = TK_ERROR;
                    } else {
                        token.tokenType = TK_ID;
                    }
                    token.lineNumber = startLine;
                    return token;
                }
                
            case S30:  /* After underscore */
                c = getNextChar(tb);
                if (isLET(c)) {
                    state = S31;
                } else {
                    retract(tb, 1);
                    token.tokenType = TK_ERROR;
                    strcpy(token.lexeme, "_");
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S31:  /* FunID letter zone */
                c = getNextChar(tb);
                if (isLET(c)) {
                    continue;
                } else if (isDIG(c)) {
                    state = S32;
                } else {
                    retract(tb, 1);
                    getLexeme(tb, token.lexeme);
                    /* Check for _main */
                    if (strcmp(token.lexeme, "_main") == 0) {
                        token.tokenType = TK_MAIN;
                    } else if (strlen(token.lexeme) <= 30) {
                        token.tokenType = TK_FUNID;
                    } else {
                        token.tokenType = TK_ERROR;
                    }
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S32:  /* FunID digit zone */
                c = getNextChar(tb);
                if (isDIG(c)) {
                    continue;
                } else {
                    retract(tb, 1);
                    getLexeme(tb, token.lexeme);
                    if (strlen(token.lexeme) <= 30) {
                        token.tokenType = TK_FUNID;
                    } else {
                        token.tokenType = TK_ERROR;
                    }
                    token.lineNumber = startLine;
                    return token;
                }
                
            case S40:  /* After # */
                c = getNextChar(tb);
                if (isLOW(c)) {
                    state = S41;
                } else {
                    retract(tb, 1);
                    token.tokenType = TK_ERROR;
                    strcpy(token.lexeme, "#");
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S41:  /* RUID body */
                c = getNextChar(tb);
                if (isLOW(c)) {
                    continue;
                } else {
                    retract(tb, 1);
                    getLexeme(tb, token.lexeme);
                    token.tokenType = TK_RUID;
                    token.lineNumber = startLine;
                    return token;
                }
                
            case S50:  /* Integer */
                c = getNextChar(tb);
                if (isDIG(c)) {
                    continue;
                } else if (c == '.') {
                    state = S51;
                } else {
                    retract(tb, 1);
                    getLexeme(tb, token.lexeme);
                    token.tokenType = TK_NUM;
                    token.value.intValue = atoi(token.lexeme);
                    token.hasValue = 1;
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S51:  /* Dot after integer */
                c = getNextChar(tb);
                if (isDIG(c)) {
                    state = S52;
                } else {
                    /* Error: malformed number like 23. */
                    retract(tb, 1);
                    getLexeme(tb, token.lexeme);
                    token.tokenType = TK_ERROR;
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S52:  /* Real fraction digit 1 */
                c = getNextChar(tb);
                if (isDIG(c)) {
                    state = S53;
                } else {
                    /* Error: only one digit after dot like 23.4 */
                    retract(tb, 1);
                    getLexeme(tb, token.lexeme);
                    token.tokenType = TK_ERROR;
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S53:  /* Real fraction digit 2 */
                c = getNextChar(tb);
                if (c == 'E') {
                    state = S54;
                } else {
                    retract(tb, 1);
                    getLexeme(tb, token.lexeme);
                    token.tokenType = TK_RNUM;
                    token.value.realValue = atof(token.lexeme);
                    token.hasValue = 1;
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S54:  /* After E */
                c = getNextChar(tb);
                if (c == '+' || c == '-') {
                    state = S55;
                } else if (isDIG(c)) {
                    state = S56;
                } else {
                    /* Error: malformed exponent */
                    retract(tb, 1);
                    getLexeme(tb, token.lexeme);
                    token.tokenType = TK_ERROR;
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S55:  /* After sign */
                c = getNextChar(tb);
                if (isDIG(c)) {
                    state = S56;
                } else {
                    /* Error: no digit after sign */
                    retract(tb, 1);
                    getLexeme(tb, token.lexeme);
                    token.tokenType = TK_ERROR;
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S56:  /* Exponent digit 1 */
                c = getNextChar(tb);
                if (isDIG(c)) {
                    state = S57;
                } else {
                    /* Error: only one exponent digit */
                    retract(tb, 1);
                    getLexeme(tb, token.lexeme);
                    token.tokenType = TK_ERROR;
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S57:  /* Exponent digit 2 */
                c = getNextChar(tb);
                retract(tb, 1);
                getLexeme(tb, token.lexeme);
                token.tokenType = TK_RNUM;
                token.value.realValue = atof(token.lexeme);
                token.hasValue = 1;
                token.lineNumber = startLine;
                return token;
                
            case S60:  /* After < */
                c = getNextChar(tb);
                if (c == '=') {
                    token.tokenType = TK_LE;
                    strcpy(token.lexeme, "<=");
                    token.lineNumber = startLine;
                    return token;
                } else if (c == '-') {
                    state = S61;
                } else {
                    retract(tb, 1);
                    token.tokenType = TK_LT;
                    strcpy(token.lexeme, "<");
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S61:  /* After <- */
                c = getNextChar(tb);
                if (c == '-') {
                    state = S62;
                } else {
                    /* Error: <- is not valid */
                    retract(tb, 1);
                    token.tokenType = TK_ERROR;
                    strcpy(token.lexeme, "<-");
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S62:  /* After <-- */
                c = getNextChar(tb);
                if (c == '-') {
                    token.tokenType = TK_ASSIGNOP;
                    strcpy(token.lexeme, "<---");
                    token.lineNumber = startLine;
                    return token;
                } else {
                    /* Error: <-- is not valid */
                    retract(tb, 1);
                    token.tokenType = TK_ERROR;
                    strcpy(token.lexeme, "<--");
                    token.lineNumber = startLine;
                    return token;
                }
                
            case S63:  /* After > */
                c = getNextChar(tb);
                if (c == '=') {
                    token.tokenType = TK_GE;
                    strcpy(token.lexeme, ">=");
                    token.lineNumber = startLine;
                    return token;
                } else {
                    retract(tb, 1);
                    token.tokenType = TK_GT;
                    strcpy(token.lexeme, ">");
                    token.lineNumber = startLine;
                    return token;
                }
                
            case S64:  /* After = */
                c = getNextChar(tb);
                if (c == '=') {
                    token.tokenType = TK_EQ;
                    strcpy(token.lexeme, "==");
                    token.lineNumber = startLine;
                    return token;
                } else {
                    /* Error: single = */
                    retract(tb, 1);
                    token.tokenType = TK_ERROR;
                    strcpy(token.lexeme, "=");
                    token.lineNumber = startLine;
                    return token;
                }
                
            case S65:  /* After ! */
                c = getNextChar(tb);
                if (c == '=') {
                    token.tokenType = TK_NE;
                    strcpy(token.lexeme, "!=");
                    token.lineNumber = startLine;
                    return token;
                } else {
                    /* Error: single ! */
                    retract(tb, 1);
                    token.tokenType = TK_ERROR;
                    strcpy(token.lexeme, "!");
                    token.lineNumber = startLine;
                    return token;
                }
                
            case S66:  /* After & */
                c = getNextChar(tb);
                if (c == '&') {
                    state = S67;
                } else {
                    /* Error: single & */
                    retract(tb, 1);
                    token.tokenType = TK_ERROR;
                    strcpy(token.lexeme, "&");
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S67:  /* After && */
                c = getNextChar(tb);
                if (c == '&') {
                    token.tokenType = TK_AND;
                    strcpy(token.lexeme, "&&&");
                    token.lineNumber = startLine;
                    return token;
                } else {
                    /* Error: && */
                    retract(tb, 1);
                    token.tokenType = TK_ERROR;
                    strcpy(token.lexeme, "&&");
                    token.lineNumber = startLine;
                    return token;
                }
                
            case S68:  /* After @ */
                c = getNextChar(tb);
                if (c == '@') {
                    state = S69;
                } else {
                    /* Error: single @ */
                    retract(tb, 1);
                    token.tokenType = TK_ERROR;
                    strcpy(token.lexeme, "@");
                    token.lineNumber = startLine;
                    return token;
                }
                break;
                
            case S69:  /* After @@ */
                c = getNextChar(tb);
                if (c == '@') {
                    token.tokenType = TK_OR;
                    strcpy(token.lexeme, "@@@");
                    token.lineNumber = startLine;
                    return token;
                } else {
                    /* Error: @@ */
                    retract(tb, 1);
                    token.tokenType = TK_ERROR;
                    strcpy(token.lexeme, "@@");
                    token.lineNumber = startLine;
                    return token;
                }
                
            default:
                /* Should not reach here */
                token.tokenType = TK_ERROR;
                strcpy(token.lexeme, "UNKNOWN");
                token.lineNumber = startLine;
                return token;
        }
    }
}

/**
 * Get stream - wrapper function for compatibility
 */
FILE* getStream(FILE *fp) {
    return fp;
}

/**
 * Remove comments from source file
 */
void removeComments(char *testcaseFile, char *cleanFile) {
    FILE *input = fopen(testcaseFile, "r");
    FILE *output = fopen(cleanFile, "w");
    
    if (!input) {
        fprintf(stderr, "Error: Cannot open input file %s\n", testcaseFile);
        return;
    }
    
    if (!output) {
        fprintf(stderr, "Error: Cannot create output file %s\n", cleanFile);
        fclose(input);
        return;
    }
    
    int c;
    int inComment = 0;
    
    while ((c = fgetc(input)) != EOF) {
        if (c == '%') {
            inComment = 1;
        }
        
        if (!inComment) {
            fputc(c, output);
        }
        
        if (inComment && c == '\n') {
            inComment = 0;
            fputc(c, output);  // Keep newline
        }
    }
    
    fclose(input);
    fclose(output);
    
    printf("Comments removed successfully. Clean code written to %s\n", cleanFile);
}

/**
 * Get token name as string
 */
const char* getTokenName(TokenType token) {
    switch (token) {
        case TK_ASSIGNOP: return "TK_ASSIGNOP";
        case TK_COMMENT: return "TK_COMMENT";
        case TK_FIELDID: return "TK_FIELDID";
        case TK_ID: return "TK_ID";
        case TK_NUM: return "TK_NUM";
        case TK_RNUM: return "TK_RNUM";
        case TK_FUNID: return "TK_FUNID";
        case TK_RUID: return "TK_RUID";
        case TK_WITH: return "TK_WITH";
        case TK_PARAMETERS: return "TK_PARAMETERS";
        case TK_END: return "TK_END";
        case TK_WHILE: return "TK_WHILE";
        case TK_UNION: return "TK_UNION";
        case TK_ENDUNION: return "TK_ENDUNION";
        case TK_DEFINETYPE: return "TK_DEFINETYPE";
        case TK_AS: return "TK_AS";
        case TK_TYPE: return "TK_TYPE";
        case TK_MAIN: return "TK_MAIN";
        case TK_GLOBAL: return "TK_GLOBAL";
        case TK_PARAMETER: return "TK_PARAMETER";
        case TK_LIST: return "TK_LIST";
        case TK_SQL: return "TK_SQL";
        case TK_SQR: return "TK_SQR";
        case TK_INPUT: return "TK_INPUT";
        case TK_OUTPUT: return "TK_OUTPUT";
        case TK_INT: return "TK_INT";
        case TK_REAL: return "TK_REAL";
        case TK_COMMA: return "TK_COMMA";
        case TK_SEM: return "TK_SEM";
        case TK_COLON: return "TK_COLON";
        case TK_DOT: return "TK_DOT";
        case TK_ENDWHILE: return "TK_ENDWHILE";
        case TK_OP: return "TK_OP";
        case TK_CL: return "TK_CL";
        case TK_IF: return "TK_IF";
        case TK_THEN: return "TK_THEN";
        case TK_ENDIF: return "TK_ENDIF";
        case TK_READ: return "TK_READ";
        case TK_WRITE: return "TK_WRITE";
        case TK_RETURN: return "TK_RETURN";
        case TK_PLUS: return "TK_PLUS";
        case TK_MINUS: return "TK_MINUS";
        case TK_MUL: return "TK_MUL";
        case TK_DIV: return "TK_DIV";
        case TK_CALL: return "TK_CALL";
        case TK_RECORD: return "TK_RECORD";
        case TK_ENDRECORD: return "TK_ENDRECORD";
        case TK_ELSE: return "TK_ELSE";
        case TK_AND: return "TK_AND";
        case TK_OR: return "TK_OR";
        case TK_NOT: return "TK_NOT";
        case TK_LT: return "TK_LT";
        case TK_LE: return "TK_LE";
        case TK_EQ: return "TK_EQ";
        case TK_GT: return "TK_GT";
        case TK_GE: return "TK_GE";
        case TK_NE: return "TK_NE";
        case TK_EOF: return "TK_EOF";
        case TK_ERROR: return "TK_ERROR";
        case TK_EPSILON: return "TK_EPSILON";
        default: return "UNKNOWN";
    }
}

/**
 * Print token (for debugging)
 */
void printToken(tokenInfo token) {
    printf("%-20s %-30s %d", getTokenName(token.tokenType), token.lexeme, token.lineNumber);
    if (token.hasValue) {
        if (token.tokenType == TK_NUM) {
            printf(" %-15d", token.value.intValue);
        } else if (token.tokenType == TK_RNUM) {
            printf(" %-15.2f", token.value.realValue);
        }
    }
    printf("\n");
}

/**
 * Free twin buffer
 */
void freeTwinBuffer(twinBuffer *tb) {
    if (tb) {
        if (tb->fp) {
            fclose(tb->fp);
            tb->fp = NULL;
        }
        free(tb);
    }
}
