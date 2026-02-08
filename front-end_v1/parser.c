/**
 * parser.c
 * Syntax Analyzer Implementation
 * CS F363 Compiler Design Project
 * 
 * Implements LL(1) predictive parser with:
 * - FIRST and FOLLOW set computation
 * - Predictive parse table creation
 * - Parse tree construction
 * - Comprehensive error reporting
 */

#include "parserDef.h"
#include "parser.h"
#include <string.h>

/* Global grammar rules count */
static int g_ruleCount = 0;

/**
 * Get non-terminal name as string
 */
const char* getNonTerminalName(NonTerminal nt) {
    switch (nt) {
        case NT_PROGRAM: return "<program>";
        case NT_MAINFUNCTION: return "<mainFunction>";
        case NT_OTHERFUNCTIONS: return "<otherFunctions>";
        case NT_FUNCTION: return "<function>";
        case NT_INPUT_PAR: return "<input_par>";
        case NT_OUTPUT_PAR: return "<output_par>";
        case NT_PARAMETER_LIST: return "<parameter_list>";
        case NT_DATATYPE: return "<dataType>";
        case NT_PRIMITIVEDATATYPE: return "<primitiveDatatype>";
        case NT_CONSTRUCTEDDATATYPE: return "<constructedDatatype>";
        case NT_REMAINING_LIST: return "<remaining_list>";
        case NT_STMTS: return "<stmts>";
        case NT_TYPEDEFINITIONS: return "<typeDefinitions>";
        case NT_TYPEDEFINITION: return "<typeDefinition>";
        case NT_FIELDDEFINITIONS: return "<fieldDefinitions>";
        case NT_FIELDDEFINITION: return "<fieldDefinition>";
        case NT_MOREFIELDS: return "<moreFields>";
        case NT_DECLARATIONS: return "<declarations>";
        case NT_DECLARATION: return "<declaration>";
        case NT_GLOBAL_OR_NOT: return "<global_or_not>";
        case NT_OTHERSTMTS: return "<otherStmts>";
        case NT_STMT: return "<stmt>";
        case NT_ASSIGNMENTSTMT: return "<assignmentStmt>";
        case NT_SINGLEORRECID: return "<singleOrRecId>";
        case NT_OPTIONALDOT: return "<optionalDot>";
        case NT_FUNCALLSTMT: return "<funCallStmt>";
        case NT_OUTPUTPARAMETERS: return "<outputParameters>";
        case NT_INPUTPARAMETERS: return "<inputParameters>";
        case NT_ITERATIVESTMT: return "<iterativeStmt>";
        case NT_CONDITIONALSTMT: return "<conditionalStmt>";
        case NT_IOSTMT: return "<ioStmt>";
        case NT_ARITHMETICEXPRESSION: return "<arithmeticExpression>";
        case NT_TERM: return "<term>";
        case NT_N7: return "<N7>";
        case NT_OPERATOR: return "<operator>";
        case NT_BOOLEANEXPRESSION: return "<booleanExpression>";
        case NT_VAR: return "<var>";
        case NT_LOGICALOP: return "<logicalOp>";
        case NT_RELATIONALOP: return "<relationalOp>";
        case NT_RETURNSTMT: return "<returnStmt>";
        case NT_OPTIONALRETURN: return "<optionalReturn>";
        case NT_IDLIST: return "<idList>";
        case NT_MORE_IDS: return "<more_ids>";
        case NT_DEFINETYPESTMT: return "<definetypestmt>";
        case NT_A: return "<A>";
        default: return "<UNKNOWN>";
    }
}

/**
 * Initialize grammar with all production rules
 * Note: Grammar needs to be LL(1) compatible
 * This implements the modified grammar from the specification
 */
grammar* initializeGrammar() {
    grammar *G = (grammar*)malloc(sizeof(grammar));
    G->ruleCount = 0;
    G->startSymbol = NT_PROGRAM;
    
    /* Helper macros for adding rules */
    #define START_RULE(nt) do { \
        G->rules[G->ruleCount].lhs = nt; \
        G->rules[G->ruleCount].rhsCount = 0; \
        G->rules[G->ruleCount].ruleNumber = G->ruleCount + 1;
    
    #define ADD_TERMINAL(tok) do { \
        int idx = G->rules[G->ruleCount].rhsCount++; \
        G->rules[G->ruleCount].rhs[idx].type = SYMBOL_TERMINAL; \
        G->rules[G->ruleCount].rhs[idx].symbol.terminal = tok; \
    } while(0)
    
    #define ADD_NONTERMINAL(nt) do { \
        int idx = G->rules[G->ruleCount].rhsCount++; \
        G->rules[G->ruleCount].rhs[idx].type = SYMBOL_NON_TERMINAL; \
        G->rules[G->ruleCount].rhs[idx].symbol.nonTerminal = nt; \
    } while(0)
    
    #define END_RULE() G->ruleCount++; } while(0)
    
    /* Rule 1: <program> -> <otherFunctions> <mainFunction> */
    START_RULE(NT_PROGRAM);
    ADD_NONTERMINAL(NT_OTHERFUNCTIONS);
    ADD_NONTERMINAL(NT_MAINFUNCTION);
    END_RULE();
    
    /* Rule 2: <mainFunction> -> TK_MAIN <stmts> TK_END */
    START_RULE(NT_MAINFUNCTION);
    ADD_TERMINAL(TK_MAIN);
    ADD_NONTERMINAL(NT_STMTS);
    ADD_TERMINAL(TK_END);
    END_RULE();
    
    /* Rule 3: <otherFunctions> -> <function> <otherFunctions> */
    START_RULE(NT_OTHERFUNCTIONS);
    ADD_NONTERMINAL(NT_FUNCTION);
    ADD_NONTERMINAL(NT_OTHERFUNCTIONS);
    END_RULE();
    
    /* Rule 4: <otherFunctions> -> ε */
    START_RULE(NT_OTHERFUNCTIONS);
    ADD_TERMINAL(TK_EPSILON);
    END_RULE();
    
    /* Rule 5: <function> -> TK_FUNID <input_par> <output_par> TK_SEM <stmts> TK_END */
    START_RULE(NT_FUNCTION);
    ADD_TERMINAL(TK_FUNID);
    ADD_NONTERMINAL(NT_INPUT_PAR);
    ADD_NONTERMINAL(NT_OUTPUT_PAR);
    ADD_TERMINAL(TK_SEM);
    ADD_NONTERMINAL(NT_STMTS);
    ADD_TERMINAL(TK_END);
    END_RULE();
    
    /* Rule 6: <input_par> -> TK_INPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR */
    START_RULE(NT_INPUT_PAR);
    ADD_TERMINAL(TK_INPUT);
    ADD_TERMINAL(TK_PARAMETER);
    ADD_TERMINAL(TK_LIST);
    ADD_TERMINAL(TK_SQL);
    ADD_NONTERMINAL(NT_PARAMETER_LIST);
    ADD_TERMINAL(TK_SQR);
    END_RULE();
    
    /* Rule 7: <output_par> -> TK_OUTPUT TK_PARAMETER TK_LIST TK_SQL <parameter_list> TK_SQR */
    START_RULE(NT_OUTPUT_PAR);
    ADD_TERMINAL(TK_OUTPUT);
    ADD_TERMINAL(TK_PARAMETER);
    ADD_TERMINAL(TK_LIST);
    ADD_TERMINAL(TK_SQL);
    ADD_NONTERMINAL(NT_PARAMETER_LIST);
    ADD_TERMINAL(TK_SQR);
    END_RULE();
    
    /* Rule 8: <output_par> -> ε */
    START_RULE(NT_OUTPUT_PAR);
    ADD_TERMINAL(TK_EPSILON);
    END_RULE();
    
    /* Rule 9: <parameter_list> -> <dataType> TK_ID <remaining_list> */
    START_RULE(NT_PARAMETER_LIST);
    ADD_NONTERMINAL(NT_DATATYPE);
    ADD_TERMINAL(TK_ID);
    ADD_NONTERMINAL(NT_REMAINING_LIST);
    END_RULE();
    
    /* Rule 10: <dataType> -> <primitiveDatatype> */
    START_RULE(NT_DATATYPE);
    ADD_NONTERMINAL(NT_PRIMITIVEDATATYPE);
    END_RULE();
    
    /* Rule 11: <dataType> -> <constructedDatatype> */
    START_RULE(NT_DATATYPE);
    ADD_NONTERMINAL(NT_CONSTRUCTEDDATATYPE);
    END_RULE();
    
    /* Rule 12: <primitiveDatatype> -> TK_INT */
    START_RULE(NT_PRIMITIVEDATATYPE);
    ADD_TERMINAL(TK_INT);
    END_RULE();
    
    /* Rule 13: <primitiveDatatype> -> TK_REAL */
    START_RULE(NT_PRIMITIVEDATATYPE);
    ADD_TERMINAL(TK_REAL);
    END_RULE();
    
    /* Rule 14: <constructedDatatype> -> TK_RECORD TK_RUID */
    START_RULE(NT_CONSTRUCTEDDATATYPE);
    ADD_TERMINAL(TK_RECORD);
    ADD_TERMINAL(TK_RUID);
    END_RULE();
    
    /* Rule 15: <constructedDatatype> -> TK_UNION TK_RUID */
    START_RULE(NT_CONSTRUCTEDDATATYPE);
    ADD_TERMINAL(TK_UNION);
    ADD_TERMINAL(TK_RUID);
    END_RULE();
    
    /* Rule 16: <remaining_list> -> TK_COMMA <parameter_list> */
    START_RULE(NT_REMAINING_LIST);
    ADD_TERMINAL(TK_COMMA);
    ADD_NONTERMINAL(NT_PARAMETER_LIST);
    END_RULE();
    
    /* Rule 17: <remaining_list> -> ε */
    START_RULE(NT_REMAINING_LIST);
    ADD_TERMINAL(TK_EPSILON);
    END_RULE();
    
    /* Rule 18: <stmts> -> <typeDefinitions> <declarations> <otherStmts> <returnStmt> */
    START_RULE(NT_STMTS);
    ADD_NONTERMINAL(NT_TYPEDEFINITIONS);
    ADD_NONTERMINAL(NT_DECLARATIONS);
    ADD_NONTERMINAL(NT_OTHERSTMTS);
    ADD_NONTERMINAL(NT_RETURNSTMT);
    END_RULE();
    
    /* Rule 19: <typeDefinitions> -> <typeDefinition> <typeDefinitions> */
    START_RULE(NT_TYPEDEFINITIONS);
    ADD_NONTERMINAL(NT_TYPEDEFINITION);
    ADD_NONTERMINAL(NT_TYPEDEFINITIONS);
    END_RULE();
    
    /* Rule 20: <typeDefinitions> -> ε */
    START_RULE(NT_TYPEDEFINITIONS);
    ADD_TERMINAL(TK_EPSILON);
    END_RULE();
    
    /* Rule 21: <typeDefinition> -> TK_RECORD TK_RUID <fieldDefinitions> TK_ENDRECORD */
    START_RULE(NT_TYPEDEFINITION);
    ADD_TERMINAL(TK_RECORD);
    ADD_TERMINAL(TK_RUID);
    ADD_NONTERMINAL(NT_FIELDDEFINITIONS);
    ADD_TERMINAL(TK_ENDRECORD);
    END_RULE();
    
    /* Rule 22: <typeDefinition> -> TK_UNION TK_RUID <fieldDefinitions> TK_ENDUNION */
    START_RULE(NT_TYPEDEFINITION);
    ADD_TERMINAL(TK_UNION);
    ADD_TERMINAL(TK_RUID);
    ADD_NONTERMINAL(NT_FIELDDEFINITIONS);
    ADD_TERMINAL(TK_ENDUNION);
    END_RULE();
    
    /* Rule 23: <fieldDefinitions> -> <fieldDefinition> <fieldDefinition> <moreFields> */
    START_RULE(NT_FIELDDEFINITIONS);
    ADD_NONTERMINAL(NT_FIELDDEFINITION);
    ADD_NONTERMINAL(NT_FIELDDEFINITION);
    ADD_NONTERMINAL(NT_MOREFIELDS);
    END_RULE();
    
    /* Rule 24: <fieldDefinition> -> TK_TYPE <primitiveDatatype> TK_COLON TK_FIELDID TK_SEM */
    START_RULE(NT_FIELDDEFINITION);
    ADD_TERMINAL(TK_TYPE);
    ADD_NONTERMINAL(NT_PRIMITIVEDATATYPE);
    ADD_TERMINAL(TK_COLON);
    ADD_TERMINAL(TK_FIELDID);
    ADD_TERMINAL(TK_SEM);
    END_RULE();
    
    /* Rule 25: <moreFields> -> <fieldDefinition> <moreFields> */
    START_RULE(NT_MOREFIELDS);
    ADD_NONTERMINAL(NT_FIELDDEFINITION);
    ADD_NONTERMINAL(NT_MOREFIELDS);
    END_RULE();
    
    /* Rule 26: <moreFields> -> ε */
    START_RULE(NT_MOREFIELDS);
    ADD_TERMINAL(TK_EPSILON);
    END_RULE();
    
    /* Rule 27: <declarations> -> <declaration> <declarations> */
    START_RULE(NT_DECLARATIONS);
    ADD_NONTERMINAL(NT_DECLARATION);
    ADD_NONTERMINAL(NT_DECLARATIONS);
    END_RULE();
    
    /* Rule 28: <declarations> -> ε */
    START_RULE(NT_DECLARATIONS);
    ADD_TERMINAL(TK_EPSILON);
    END_RULE();
    
    /* Rule 29: <declaration> -> TK_TYPE <dataType> TK_COLON TK_ID <global_or_not> TK_SEM */
    START_RULE(NT_DECLARATION);
    ADD_TERMINAL(TK_TYPE);
    ADD_NONTERMINAL(NT_DATATYPE);
    ADD_TERMINAL(TK_COLON);
    ADD_TERMINAL(TK_ID);
    ADD_NONTERMINAL(NT_GLOBAL_OR_NOT);
    ADD_TERMINAL(TK_SEM);
    END_RULE();
    
    /* Rule 30: <global_or_not> -> TK_GLOBAL */
    START_RULE(NT_GLOBAL_OR_NOT);
    ADD_TERMINAL(TK_GLOBAL);
    END_RULE();
    
    /* Rule 31: <global_or_not> -> ε */
    START_RULE(NT_GLOBAL_OR_NOT);
    ADD_TERMINAL(TK_EPSILON);
    END_RULE();
    
    /* Rule 32: <otherStmts> -> <stmt> <otherStmts> */
    START_RULE(NT_OTHERSTMTS);
    ADD_NONTERMINAL(NT_STMT);
    ADD_NONTERMINAL(NT_OTHERSTMTS);
    END_RULE();
    
    /* Rule 33: <otherStmts> -> ε */
    START_RULE(NT_OTHERSTMTS);
    ADD_TERMINAL(TK_EPSILON);
    END_RULE();
    
    /* Rule 34: <stmt> -> <assignmentStmt> */
    START_RULE(NT_STMT);
    ADD_NONTERMINAL(NT_ASSIGNMENTSTMT);
    END_RULE();
    
    /* Rule 35: <stmt> -> <iterativeStmt> */
    START_RULE(NT_STMT);
    ADD_NONTERMINAL(NT_ITERATIVESTMT);
    END_RULE();
    
    /* Rule 36: <stmt> -> <conditionalStmt> */
    START_RULE(NT_STMT);
    ADD_NONTERMINAL(NT_CONDITIONALSTMT);
    END_RULE();
    
    /* Rule 37: <stmt> -> <ioStmt> */
    START_RULE(NT_STMT);
    ADD_NONTERMINAL(NT_IOSTMT);
    END_RULE();
    
    /* Rule 38: <stmt> -> <funCallStmt> */
    START_RULE(NT_STMT);
    ADD_NONTERMINAL(NT_FUNCALLSTMT);
    END_RULE();
    
    /* Rule 39: <assignmentStmt> -> <singleOrRecId> TK_ASSIGNOP <arithmeticExpression> TK_SEM */
    START_RULE(NT_ASSIGNMENTSTMT);
    ADD_NONTERMINAL(NT_SINGLEORRECID);
    ADD_TERMINAL(TK_ASSIGNOP);
    ADD_NONTERMINAL(NT_ARITHMETICEXPRESSION);
    ADD_TERMINAL(TK_SEM);
    END_RULE();
    
    /* Rule 40: <singleOrRecId> -> TK_ID <optionalDot> */
    START_RULE(NT_SINGLEORRECID);
    ADD_TERMINAL(TK_ID);
    ADD_NONTERMINAL(NT_OPTIONALDOT);
    END_RULE();
    
    /* Rule 41: <optionalDot> -> TK_DOT TK_FIELDID */
    START_RULE(NT_OPTIONALDOT);
    ADD_TERMINAL(TK_DOT);
    ADD_TERMINAL(TK_FIELDID);
    END_RULE();
    
    /* Rule 42: <optionalDot> -> ε */
    START_RULE(NT_OPTIONALDOT);
    ADD_TERMINAL(TK_EPSILON);
    END_RULE();
    
    /* Rule 43: <funCallStmt> -> <outputParameters> TK_CALL TK_FUNID TK_WITH TK_PARAMETERS <inputParameters> TK_SEM */
    START_RULE(NT_FUNCALLSTMT);
    ADD_NONTERMINAL(NT_OUTPUTPARAMETERS);
    ADD_TERMINAL(TK_CALL);
    ADD_TERMINAL(TK_FUNID);
    ADD_TERMINAL(TK_WITH);
    ADD_TERMINAL(TK_PARAMETERS);
    ADD_NONTERMINAL(NT_INPUTPARAMETERS);
    ADD_TERMINAL(TK_SEM);
    END_RULE();
    
    /* Rule 44: <outputParameters> -> TK_SQL <idList> TK_SQR TK_ASSIGNOP */
    START_RULE(NT_OUTPUTPARAMETERS);
    ADD_TERMINAL(TK_SQL);
    ADD_NONTERMINAL(NT_IDLIST);
    ADD_TERMINAL(TK_SQR);
    ADD_TERMINAL(TK_ASSIGNOP);
    END_RULE();
    
    /* Rule 45: <outputParameters> -> ε */
    START_RULE(NT_OUTPUTPARAMETERS);
    ADD_TERMINAL(TK_EPSILON);
    END_RULE();
    
    /* Rule 46: <inputParameters> -> TK_SQL <idList> TK_SQR */
    START_RULE(NT_INPUTPARAMETERS);
    ADD_TERMINAL(TK_SQL);
    ADD_NONTERMINAL(NT_IDLIST);
    ADD_TERMINAL(TK_SQR);
    END_RULE();
    
    /* Rule 46: <iterativeStmt> -> TK_WHILE TK_OP <booleanExpression> TK_CL <stmt> <otherStmts> TK_ENDWHILE */
    START_RULE(NT_ITERATIVESTMT);
    ADD_TERMINAL(TK_WHILE);
    ADD_TERMINAL(TK_OP);
    ADD_NONTERMINAL(NT_BOOLEANEXPRESSION);
    ADD_TERMINAL(TK_CL);
    ADD_NONTERMINAL(NT_STMT);
    ADD_NONTERMINAL(NT_OTHERSTMTS);
    ADD_TERMINAL(TK_ENDWHILE);
    END_RULE();
    
    /* Rule 47: <conditionalStmt> -> TK_IF <booleanExpression> TK_THEN <stmt> <otherStmts> TK_ELSE <stmt> <otherStmts> TK_ENDIF */
    START_RULE(NT_CONDITIONALSTMT);
    ADD_TERMINAL(TK_IF);
    ADD_NONTERMINAL(NT_BOOLEANEXPRESSION);
    ADD_TERMINAL(TK_THEN);
    ADD_NONTERMINAL(NT_STMT);
    ADD_NONTERMINAL(NT_OTHERSTMTS);
    ADD_TERMINAL(TK_ELSE);
    ADD_NONTERMINAL(NT_STMT);
    ADD_NONTERMINAL(NT_OTHERSTMTS);
    ADD_TERMINAL(TK_ENDIF);
    END_RULE();
    
    /* Rule 48: <conditionalStmt> -> TK_IF <booleanExpression> TK_THEN <stmt> <otherStmts> TK_ENDIF */
    START_RULE(NT_CONDITIONALSTMT);
    ADD_TERMINAL(TK_IF);
    ADD_NONTERMINAL(NT_BOOLEANEXPRESSION);
    ADD_TERMINAL(TK_THEN);
    ADD_NONTERMINAL(NT_STMT);
    ADD_NONTERMINAL(NT_OTHERSTMTS);
    ADD_TERMINAL(TK_ENDIF);
    END_RULE();
    
    /* Rule 49: <ioStmt> -> TK_READ TK_OP <var> TK_CL TK_SEM */
    START_RULE(NT_IOSTMT);
    ADD_TERMINAL(TK_READ);
    ADD_TERMINAL(TK_OP);
    ADD_NONTERMINAL(NT_VAR);
    ADD_TERMINAL(TK_CL);
    ADD_TERMINAL(TK_SEM);
    END_RULE();
    
    /* Rule 50: <ioStmt> -> TK_WRITE TK_OP <var> TK_CL TK_SEM */
    START_RULE(NT_IOSTMT);
    ADD_TERMINAL(TK_WRITE);
    ADD_TERMINAL(TK_OP);
    ADD_NONTERMINAL(NT_VAR);
    ADD_TERMINAL(TK_CL);
    ADD_TERMINAL(TK_SEM);
    END_RULE();
    
    /* Rule 51: <var> -> TK_ID */
    START_RULE(NT_VAR);
    ADD_TERMINAL(TK_ID);
    END_RULE();
    
    /* Rule 52: <var> -> TK_NUM */
    START_RULE(NT_VAR);
    ADD_TERMINAL(TK_NUM);
    END_RULE();
    
    /* Rule 53: <var> -> TK_RNUM */
    START_RULE(NT_VAR);
    ADD_TERMINAL(TK_RNUM);
    END_RULE();
    
    /* Arithmetic expression rules - left-factored with operator handling */
    /* Rule 54: <arithmeticExpression> -> <term> <N7> */
    START_RULE(NT_ARITHMETICEXPRESSION);
    ADD_NONTERMINAL(NT_TERM);
    ADD_NONTERMINAL(NT_N7);
    END_RULE();
    
    /* Rule 55: <N7> -> <operator> <term> <N7> */
    START_RULE(NT_N7);
    ADD_NONTERMINAL(NT_OPERATOR);
    ADD_NONTERMINAL(NT_TERM);
    ADD_NONTERMINAL(NT_N7);
    END_RULE();
    
    /* Rule 56: <N7> -> ε */
    START_RULE(NT_N7);
    ADD_TERMINAL(TK_EPSILON);
    END_RULE();
    
    /* Rule 57: <term> -> TK_OP <arithmeticExpression> TK_CL */
    START_RULE(NT_TERM);
    ADD_TERMINAL(TK_OP);
    ADD_NONTERMINAL(NT_ARITHMETICEXPRESSION);
    ADD_TERMINAL(TK_CL);
    END_RULE();
    
    /* Rule 58: <term> -> <var> */
    START_RULE(NT_TERM);
    ADD_NONTERMINAL(NT_VAR);
    END_RULE();
    
    /* Rule 59: <operator> -> TK_PLUS */
    START_RULE(NT_OPERATOR);
    ADD_TERMINAL(TK_PLUS);
    END_RULE();
    
    /* Rule 60: <operator> -> TK_MINUS */
    START_RULE(NT_OPERATOR);
    ADD_TERMINAL(TK_MINUS);
    END_RULE();
    
    /* Rule 61: <operator> -> TK_MUL */
    START_RULE(NT_OPERATOR);
    ADD_TERMINAL(TK_MUL);
    END_RULE();
    
    /* Rule 62: <operator> -> TK_DIV */
    START_RULE(NT_OPERATOR);
    ADD_TERMINAL(TK_DIV);
    END_RULE();
    
    /* Rule 63: <booleanExpression> -> TK_OP <booleanExpression> TK_CL <logicalOp> TK_OP <booleanExpression> TK_CL */
    START_RULE(NT_BOOLEANEXPRESSION);
    ADD_TERMINAL(TK_OP);
    ADD_NONTERMINAL(NT_BOOLEANEXPRESSION);
    ADD_TERMINAL(TK_CL);
    ADD_NONTERMINAL(NT_LOGICALOP);
    ADD_TERMINAL(TK_OP);
    ADD_NONTERMINAL(NT_BOOLEANEXPRESSION);
    ADD_TERMINAL(TK_CL);
    END_RULE();
    
    /* Rule 61: <booleanExpression> -> <var> <relationalOp> <var> */
    START_RULE(NT_BOOLEANEXPRESSION);
    ADD_NONTERMINAL(NT_VAR);
    ADD_NONTERMINAL(NT_RELATIONALOP);
    ADD_NONTERMINAL(NT_VAR);
    END_RULE();
    
    /* Rule 62: <booleanExpression> -> TK_NOT <booleanExpression> */
    START_RULE(NT_BOOLEANEXPRESSION);
    ADD_TERMINAL(TK_NOT);
    ADD_NONTERMINAL(NT_BOOLEANEXPRESSION);
    END_RULE();
    
    /* Rule 63: <logicalOp> -> TK_AND */
    START_RULE(NT_LOGICALOP);
    ADD_TERMINAL(TK_AND);
    END_RULE();
    
    /* Rule 64: <logicalOp> -> TK_OR */
    START_RULE(NT_LOGICALOP);
    ADD_TERMINAL(TK_OR);
    END_RULE();
    
    /* Rule 65: <relationalOp> -> TK_LT */
    START_RULE(NT_RELATIONALOP);
    ADD_TERMINAL(TK_LT);
    END_RULE();
    
    /* Rule 66: <relationalOp> -> TK_LE */
    START_RULE(NT_RELATIONALOP);
    ADD_TERMINAL(TK_LE);
    END_RULE();
    
    /* Rule 67: <relationalOp> -> TK_EQ */
    START_RULE(NT_RELATIONALOP);
    ADD_TERMINAL(TK_EQ);
    END_RULE();
    
    /* Rule 68: <relationalOp> -> TK_GT */
    START_RULE(NT_RELATIONALOP);
    ADD_TERMINAL(TK_GT);
    END_RULE();
    
    /* Rule 69: <relationalOp> -> TK_GE */
    START_RULE(NT_RELATIONALOP);
    ADD_TERMINAL(TK_GE);
    END_RULE();
    
    /* Rule 70: <relationalOp> -> TK_NE */
    START_RULE(NT_RELATIONALOP);
    ADD_TERMINAL(TK_NE);
    END_RULE();
    
    /* Rule 71: <returnStmt> -> TK_RETURN <optionalReturn> TK_SEM */
    START_RULE(NT_RETURNSTMT);
    ADD_TERMINAL(TK_RETURN);
    ADD_NONTERMINAL(NT_OPTIONALRETURN);
    ADD_TERMINAL(TK_SEM);
    END_RULE();
    
    /* Rule 72: <optionalReturn> -> TK_SQL <idList> TK_SQR */
    START_RULE(NT_OPTIONALRETURN);
    ADD_TERMINAL(TK_SQL);
    ADD_NONTERMINAL(NT_IDLIST);
    ADD_TERMINAL(TK_SQR);
    END_RULE();
    
    /* Rule 73: <optionalReturn> -> ε */
    START_RULE(NT_OPTIONALRETURN);
    ADD_TERMINAL(TK_EPSILON);
    END_RULE();
    
    /* Rule 74: <idList> -> TK_ID <more_ids> */
    START_RULE(NT_IDLIST);
    ADD_TERMINAL(TK_ID);
    ADD_NONTERMINAL(NT_MORE_IDS);
    END_RULE();
    
    /* Rule 75: <more_ids> -> TK_COMMA <idList> */
    START_RULE(NT_MORE_IDS);
    ADD_TERMINAL(TK_COMMA);
    ADD_NONTERMINAL(NT_IDLIST);
    END_RULE();
    
    /* Rule 76: <more_ids> -> ε */
    START_RULE(NT_MORE_IDS);
    ADD_TERMINAL(TK_EPSILON);
    END_RULE();
    
    g_ruleCount = G->ruleCount;
    
    return G;
}

/**
 * Helper functions for FIRST and FOLLOW computation
 */
void addToFirst(FirstSet *set, TokenType token) {
    for (int i = 0; i < set->firstCount; i++) {
        if (set->first[i] == token) return;  // Already present
    }
    if (set->firstCount < MAX_FIRST_FOLLOW) {
        set->first[set->firstCount++] = token;
    }
}

void addToFollow(FollowSet *set, TokenType token) {
    for (int i = 0; i < set->followCount; i++) {
        if (set->follow[i] == token) return;  // Already present
    }
    if (set->followCount < MAX_FIRST_FOLLOW) {
        set->follow[set->followCount++] = token;
    }
}

int hasEpsilon(NonTerminal nt, FirstAndFollow *F) {
    for (int i = 0; i < F->firstCount; i++) {
        if (F->firstSets[i].nonTerminal == nt) {
            return F->firstSets[i].hasEpsilon;
        }
    }
    return 0;
}

FirstSet* getFirstSet(NonTerminal nt, FirstAndFollow *F) {
    for (int i = 0; i < F->firstCount; i++) {
        if (F->firstSets[i].nonTerminal == nt) {
            return &F->firstSets[i];
        }
    }
    return NULL;
}

FollowSet* getFollowSet(NonTerminal nt, FirstAndFollow *F) {
    for (int i = 0; i < F->followCount; i++) {
        if (F->followSets[i].nonTerminal == nt) {
            return &F->followSets[i];
        }
    }
    return NULL;
}

/**
 * Compute FIRST sets for all non-terminals
 */
void computeFirst(NonTerminal nt, grammar *G, FirstAndFollow *F) {
    FirstSet *firstSet = getFirstSet(nt, F);
    if (!firstSet) return;
    
    /* Iterate through all rules with this non-terminal on LHS */
    for (int i = 0; i < G->ruleCount; i++) {
        if (G->rules[i].lhs != nt) continue;
        
        GrammarRule *rule = &G->rules[i];
        
        /* If rule is A -> ε */
        if (rule->rhsCount == 1 && 
            rule->rhs[0].type == SYMBOL_TERMINAL && 
            rule->rhs[0].symbol.terminal == TK_EPSILON) {
            firstSet->hasEpsilon = 1;
            continue;
        }
        
        /* Process RHS symbols */
        int allHaveEpsilon = 1;
        for (int j = 0; j < rule->rhsCount; j++) {
            GrammarSymbol *sym = &rule->rhs[j];
            
            if (sym->type == SYMBOL_TERMINAL) {
                /* Add terminal to FIRST set */
                addToFirst(firstSet, sym->symbol.terminal);
                allHaveEpsilon = 0;
                break;
            } else {
                /* Non-terminal: add its FIRST set (except epsilon) */
                FirstSet *rhsFirst = getFirstSet(sym->symbol.nonTerminal, F);
                if (rhsFirst) {
                    for (int k = 0; k < rhsFirst->firstCount; k++) {
                        addToFirst(firstSet, rhsFirst->first[k]);
                    }
                    
                    if (!rhsFirst->hasEpsilon) {
                        allHaveEpsilon = 0;
                        break;
                    }
                }
            }
        }
        
        /* If all RHS symbols have epsilon, add epsilon to FIRST */
        if (allHaveEpsilon) {
            firstSet->hasEpsilon = 1;
        }
    }
}

/**
 * Compute FOLLOW sets for all non-terminals
 */
void computeFollow(NonTerminal nt, grammar *G, FirstAndFollow *F) {
    /* Add $ to FOLLOW of start symbol */
    if (nt == G->startSymbol) {
        FollowSet *followSet = getFollowSet(nt, F);
        if (followSet) {
            addToFollow(followSet, TK_EOF);
        }
    }
    
    /* For each production A -> αBβ */
    for (int i = 0; i < G->ruleCount; i++) {
        GrammarRule *rule = &G->rules[i];
        
        for (int j = 0; j < rule->rhsCount; j++) {
            if (rule->rhs[j].type == SYMBOL_NON_TERMINAL && 
                rule->rhs[j].symbol.nonTerminal == nt) {
                
                FollowSet *followSet = getFollowSet(nt, F);
                if (!followSet) continue;
                
                /* Check if there's a β after B */
                int allBetaHaveEpsilon = 1;
                for (int k = j + 1; k < rule->rhsCount; k++) {
                    GrammarSymbol *beta = &rule->rhs[k];
                    
                    if (beta->type == SYMBOL_TERMINAL) {
                        /* Add terminal to FOLLOW */
                        addToFollow(followSet, beta->symbol.terminal);
                        allBetaHaveEpsilon = 0;
                        break;
                    } else {
                        /* Add FIRST(β) - {ε} to FOLLOW(B) */
                        FirstSet *betaFirst = getFirstSet(beta->symbol.nonTerminal, F);
                        if (betaFirst) {
                            for (int m = 0; m < betaFirst->firstCount; m++) {
                                addToFollow(followSet, betaFirst->first[m]);
                            }
                            
                            if (!betaFirst->hasEpsilon) {
                                allBetaHaveEpsilon = 0;
                                break;
                            }
                        }
                    }
                }
                
                /* If β =>* ε or β is empty, add FOLLOW(A) to FOLLOW(B) */
                if (allBetaHaveEpsilon) {
                    FollowSet *lhsFollow = getFollowSet(rule->lhs, F);
                    if (lhsFollow) {
                        for (int k = 0; k < lhsFollow->followCount; k++) {
                            addToFollow(followSet, lhsFollow->follow[k]);
                        }
                    }
                }
            }
        }
    }
}

/**
 * Compute all FIRST and FOLLOW sets
 */
FirstAndFollow computeFirstAndFollowSets(grammar *G) {
    FirstAndFollow F;
    F.firstCount = 0;
    F.followCount = 0;
    
    /* Initialize FIRST sets for all non-terminals */
    for (int i = NT_PROGRAM; i <= NT_A; i++) {
        F.firstSets[F.firstCount].nonTerminal = (NonTerminal)i;
        F.firstSets[F.firstCount].firstCount = 0;
        F.firstSets[F.firstCount].hasEpsilon = 0;
        F.firstCount++;
    }
    
    /* Initialize FOLLOW sets for all non-terminals */
    for (int i = NT_PROGRAM; i <= NT_A; i++) {
        F.followSets[F.followCount].nonTerminal = (NonTerminal)i;
        F.followSets[F.followCount].followCount = 0;
        F.followCount++;
    }
    
    /* Compute FIRST sets iteratively until no changes */
    int changed = 1;
    int iterations = 0;
    while (changed && iterations < 10) {
        changed = 0;
        int oldCounts[50];
        
        for (int i = 0; i < F.firstCount; i++) {
            oldCounts[i] = F.firstSets[i].firstCount;
        }
        
        for (int i = NT_PROGRAM; i <= NT_A; i++) {
            computeFirst((NonTerminal)i, G, &F);
        }
        
        for (int i = 0; i < F.firstCount; i++) {
            if (F.firstSets[i].firstCount != oldCounts[i]) {
                changed = 1;
                break;
            }
        }
        iterations++;
    }
    
    /* Compute FOLLOW sets iteratively until no changes */
    changed = 1;
    iterations = 0;
    while (changed && iterations < 10) {
        changed = 0;
        int oldCounts[50];
        
        for (int i = 0; i < F.followCount; i++) {
            oldCounts[i] = F.followSets[i].followCount;
        }
        
        for (int i = NT_PROGRAM; i <= NT_A; i++) {
            computeFollow((NonTerminal)i, G, &F);
        }
        
        for (int i = 0; i < F.followCount; i++) {
            if (F.followSets[i].followCount != oldCounts[i]) {
                changed = 1;
                break;
            }
        }
        iterations++;
    }
    
    return F;
}

/**
 * Create predictive parse table
 */
void createParseTable(FirstAndFollow *F, table *T, grammar *G) {
    /* Initialize table */
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 100; j++) {
            T->entries[i][j].isValid = 0;
            T->entries[i][j].ruleNumber = -1;
            T->entries[i][j].rule = NULL;
        }
    }
    
    /* For each grammar rule A -> α */
    for (int i = 0; i < G->ruleCount; i++) {
        GrammarRule *rule = &G->rules[i];
        NonTerminal A = rule->lhs;
        
        /* Get FIRST(α) */
        FirstSet *firstAlpha = (FirstSet*)malloc(sizeof(FirstSet));
        firstAlpha->firstCount = 0;
        firstAlpha->hasEpsilon = 0;
        
        /* Compute FIRST(α) */
        int allHaveEpsilon = 1;
        for (int j = 0; j < rule->rhsCount; j++) {
            GrammarSymbol *sym = &rule->rhs[j];
            
            if (sym->type == SYMBOL_TERMINAL) {
                if (sym->symbol.terminal == TK_EPSILON) {
                    firstAlpha->hasEpsilon = 1;
                } else {
                    addToFirst(firstAlpha, sym->symbol.terminal);
                }
                allHaveEpsilon = 0;
                break;
            } else {
                FirstSet *symFirst = getFirstSet(sym->symbol.nonTerminal, F);
                if (symFirst) {
                    for (int k = 0; k < symFirst->firstCount; k++) {
                        addToFirst(firstAlpha, symFirst->first[k]);
                    }
                    if (!symFirst->hasEpsilon) {
                        allHaveEpsilon = 0;
                        break;
                    }
                }
            }
        }
        
        if (allHaveEpsilon) {
            firstAlpha->hasEpsilon = 1;
        }
        
        /* For each terminal a in FIRST(α), add A -> α to M[A, a] */
        for (int j = 0; j < firstAlpha->firstCount; j++) {
            TokenType a = firstAlpha->first[j];
            T->entries[A][a].isValid = 1;
            T->entries[A][a].ruleNumber = rule->ruleNumber;
            T->entries[A][a].rule = rule;
        }
        
        /* If ε in FIRST(α), for each b in FOLLOW(A), add A -> α to M[A, b] */
        if (firstAlpha->hasEpsilon) {
            FollowSet *followA = getFollowSet(A, F);
            if (followA) {
                for (int j = 0; j < followA->followCount; j++) {
                    TokenType b = followA->follow[j];
                    T->entries[A][b].isValid = 1;
                    T->entries[A][b].ruleNumber = rule->ruleNumber;
                    T->entries[A][b].rule = rule;
                }
            }
        }
        
        free(firstAlpha);
    }
}

/**
 * Stack operations
 */
stackNode* createStackNode(GrammarSymbol symbol, parseTreeNode *treeNode) {
    stackNode *node = (stackNode*)malloc(sizeof(stackNode));
    node->symbol = symbol;
    node->treeNode = treeNode;
    node->next = NULL;
    return node;
}

void push(stackNode **top, GrammarSymbol symbol, parseTreeNode *treeNode) {
    stackNode *node = createStackNode(symbol, treeNode);
    node->next = *top;
    *top = node;
}

stackNode* pop(stackNode **top) {
    if (*top == NULL) return NULL;
    stackNode *node = *top;
    *top = (*top)->next;
    return node;
}

int isStackEmpty(stackNode *top) {
    return top == NULL;
}

GrammarSymbol topSymbol(stackNode *top) {
    if (top) return top->symbol;
    GrammarSymbol empty;
    empty.type = SYMBOL_TERMINAL;
    empty.symbol.terminal = TK_EOF;
    return empty;
}

/**
 * Parse tree operations
 */
parseTreeNode* createTreeNode(GrammarSymbol symbol) {
    parseTreeNode *node = (parseTreeNode*)malloc(sizeof(parseTreeNode));
    node->symbol = symbol;
    node->childCount = 0;
    node->parent = NULL;
    node->visited = 0;
    
    if (symbol.type == SYMBOL_TERMINAL) {
        node->isLeaf = 1;
    } else {
        node->isLeaf = 0;
    }
    
    for (int i = 0; i < MAX_CHILDREN; i++) {
        node->children[i] = NULL;
    }
    
    return node;
}

void addChild(parseTreeNode *parent, parseTreeNode *child) {
    if (parent && child && parent->childCount < MAX_CHILDREN) {
        parent->children[parent->childCount++] = child;
        child->parent = parent;
    }
}

/**
 * Main parsing function - implements predictive parsing
 */
parseTree* parseInputSourceCode(char *testcaseFile, table *T, grammar *G) {
    FILE *fp = fopen(testcaseFile, "r");
    if (!fp) {
        fprintf(stderr, "Error: Cannot open file %s\n", testcaseFile);
        return NULL;
    }
    
    twinBuffer *tb = initializeTwinBuffer(fp);
    parseTree *PT = (parseTree*)malloc(sizeof(parseTree));
    
    /* Create root node */
    GrammarSymbol startSymbol;
    startSymbol.type = SYMBOL_NON_TERMINAL;
    startSymbol.symbol.nonTerminal = G->startSymbol;
    
    PT->root = createTreeNode(startSymbol);
    
    /* Initialize stack with $ and start symbol */
    stackNode *stack = NULL;
    GrammarSymbol dollar;
    dollar.type = SYMBOL_TERMINAL;
    dollar.symbol.terminal = TK_EOF;
    push(&stack, dollar, NULL);
    push(&stack, startSymbol, PT->root);
    
    /* Get first token */
    tokenInfo currentToken = getNextToken(tb);
    
    int errorCount = 0;
    int syntaxCorrect = 1;
    
    /* Parsing loop */
    while (!isStackEmpty(stack)) {
        stackNode *topNode = pop(&stack);
        GrammarSymbol X = topNode->symbol;
        parseTreeNode *currentTreeNode = topNode->treeNode;
        
        if (X.type == SYMBOL_TERMINAL) {
            if (X.symbol.terminal == TK_EPSILON) {
                /* Epsilon production - do nothing */
                free(topNode);
                continue;
            }
            
            if (X.symbol.terminal == currentToken.tokenType) {
                /* Match */
                if (currentTreeNode) {
                    currentTreeNode->token = currentToken;
                }
                
                if (currentToken.tokenType != TK_EOF) {
                    currentToken = getNextToken(tb);
                }
            } else {
                /* Error: terminal mismatch */
                fprintf(stderr, "Line %d: Syntax error - Expected %s but found %s\n",
                        currentToken.lineNumber,
                        getTokenName(X.symbol.terminal),
                        getTokenName(currentToken.tokenType));
                errorCount++;
                syntaxCorrect = 0;
                
                /* Error recovery: skip current token */
                if (currentToken.tokenType != TK_EOF) {
                    currentToken = getNextToken(tb);
                }
            }
        } else {
            /* Non-terminal */
            NonTerminal A = X.symbol.nonTerminal;
            TokenType a = currentToken.tokenType;
            
            /* Report lexical errors */
            if (a == TK_ERROR) {
                fprintf(stderr, "Line %d: Lexical error - Invalid token '%s'\n",
                        currentToken.lineNumber, currentToken.lexeme);
                errorCount++;
                syntaxCorrect = 0;
                currentToken = getNextToken(tb);
                /* Push non-terminal back */
                push(&stack, X, currentTreeNode);
                free(topNode);
                continue;
            }
            
            /* Look up in parse table */
            if (T->entries[A][a].isValid) {
                GrammarRule *rule = T->entries[A][a].rule;
                
                /* Create children nodes and push onto stack in reverse order */
                parseTreeNode *children[MAX_RHS];
                int childCount = 0;
                
                for (int i = 0; i < rule->rhsCount; i++) {
                    parseTreeNode *child = createTreeNode(rule->rhs[i]);
                    children[childCount++] = child;
                }
                
                /* Add children to current node */
                for (int i = 0; i < childCount; i++) {
                    addChild(currentTreeNode, children[i]);
                }
                
                /* Push children onto stack in reverse order */
                for (int i = childCount - 1; i >= 0; i--) {
                    push(&stack, rule->rhs[i], children[i]);
                }
            } else {
                /* Error: no valid production */
                fprintf(stderr, "Line %d: Syntax error - Unexpected token %s (%s) for non-terminal %s\n",
                        currentToken.lineNumber,
                        getTokenName(currentToken.tokenType),
                        currentToken.lexeme,
                        getNonTerminalName(A));
                errorCount++;
                syntaxCorrect = 0;
                
                /* Error recovery: skip current token */
                if (currentToken.tokenType != TK_EOF) {
                    currentToken = getNextToken(tb);
                }
            }
        }
        
        free(topNode);
    }
    
    freeTwinBuffer(tb);
    
    if (syntaxCorrect && errorCount == 0) {
        printf("\nInput source code is syntactically correct...........\n");
    } else {
        printf("\nInput source code has %d syntax/lexical errors\n", errorCount);
    }
    
    return PT;
}

/**
 * Print inorder traversal of parse tree
 */
void printInorder(parseTreeNode *node, FILE *fp, parseTreeNode *root) {
    if (!node) return;
    
    /* Inorder: leftmost child -> parent -> remaining children */
    if (node->childCount > 0) {
        /* Visit leftmost child first */
        printInorder(node->children[0], fp, root);
    }
    
    /* Visit current node */
    if (!node->visited) {
        node->visited = 1;
        
        /* Format: lexeme currentNode lineno tokenName valueIfNumber parentNodeSymbol isLeafNode nodeSymbol */
        
        /* 1. Lexeme (or ----) */
        if (node->isLeaf && node->symbol.type == SYMBOL_TERMINAL) {
            if (node->symbol.symbol.terminal != TK_EPSILON) {
                fprintf(fp, "%-20s ", node->token.lexeme);
            } else {
                fprintf(fp, "%-20s ", "----");
            }
        } else {
            fprintf(fp, "%-20s ", "----");
        }
        
        /* 2. Current node token name or non-terminal */
        if (node->isLeaf) {
            fprintf(fp, "%-20s ", getTokenName(node->symbol.symbol.terminal));
        } else {
            fprintf(fp, "%-20s ", getNonTerminalName(node->symbol.symbol.nonTerminal));
        }
        
        /* 3. Line number */
        if (node->isLeaf && node->symbol.symbol.terminal != TK_EPSILON) {
            fprintf(fp, "%-8d ", node->token.lineNumber);
        } else {
            fprintf(fp, "%-8s ", "----");
        }
        
        /* 4. Token name */
        if (node->isLeaf) {
            fprintf(fp, "%-20s ", getTokenName(node->symbol.symbol.terminal));
        } else {
            fprintf(fp, "%-20s ", "----");
        }
        
        /* 5. Value if number */
        if (node->isLeaf && node->token.hasValue) {
            if (node->symbol.symbol.terminal == TK_NUM) {
                fprintf(fp, "%-15d ", node->token.value.intValue);
            } else if (node->symbol.symbol.terminal == TK_RNUM) {
                fprintf(fp, "%-15.2f ", node->token.value.realValue);
            } else {
                fprintf(fp, "%-15s ", "----");
            }
        } else {
            fprintf(fp, "%-15s ", "----");
        }
        
        /* 6. Parent node symbol */
        if (node == root) {
            fprintf(fp, "%-20s ", "ROOT");
        } else if (node->parent) {
            if (node->parent->symbol.type == SYMBOL_NON_TERMINAL) {
                fprintf(fp, "%-20s ", getNonTerminalName(node->parent->symbol.symbol.nonTerminal));
            } else {
                fprintf(fp, "%-20s ", getTokenName(node->parent->symbol.symbol.terminal));
            }
        } else {
            fprintf(fp, "%-20s ", "----");
        }
        
        /* 7. Is leaf node */
        fprintf(fp, "%-8s ", node->isLeaf ? "yes" : "no");
        
        /* 8. Node symbol */
        if (node->symbol.type == SYMBOL_NON_TERMINAL) {
            fprintf(fp, "%s", getNonTerminalName(node->symbol.symbol.nonTerminal));
        } else {
            fprintf(fp, "%s", "----");
        }
        
        fprintf(fp, "\n");
    }
    
    /* Visit remaining children */
    for (int i = 1; i < node->childCount; i++) {
        printInorder(node->children[i], fp, root);
    }
}

/**
 * Print parse tree to file
 */
void printParseTree(parseTree *PT, char *outfile) {
    FILE *fp = fopen(outfile, "w");
    if (!fp) {
        fprintf(stderr, "Error: Cannot create output file %s\n", outfile);
        return;
    }
    
    /* Print header */
    fprintf(fp, "%-20s %-20s %-8s %-20s %-15s %-20s %-8s %s\n",
            "LEXEME", "CURRENT_NODE", "LINENO", "TOKEN_NAME", "VALUE",
            "PARENT_NODE", "ISLEAF", "NODE_SYMBOL");
    fprintf(fp, "================================================================================================================================================\n");
    
    printInorder(PT->root, fp, PT->root);
    
    fclose(fp);
    printf("Parse tree printed to %s\n", outfile);
}

/**
 * Print parse table for verification
 */
void printParseTable(table *T, grammar *G) {
    printf("\n========== PREDICTIVE PARSE TABLE ==========\n\n");
    
    /* Print header */
    printf("%-25s", "Non-Terminal");
    TokenType terminals[] = {TK_MAIN, TK_FUNID, TK_TYPE, TK_RECORD, TK_UNION,
                             TK_INT, TK_REAL, TK_ID, TK_RETURN, TK_END,
                             TK_WHILE, TK_IF, TK_READ, TK_WRITE, TK_CALL, TK_EOF};
    
    for (int i = 0; i < 16; i++) {
        printf("%-15s", getTokenName(terminals[i]));
    }
    printf("\n");
    
    printf("=================================================================================================\n");
    
    /* Print table entries */
    for (int nt = NT_PROGRAM; nt <= NT_A; nt++) {
        printf("%-25s", getNonTerminalName((NonTerminal)nt));
        
        for (int i = 0; i < 16; i++) {
            TokenType term = terminals[i];
            if (T->entries[nt][term].isValid) {
                printf("%-15d", T->entries[nt][term].ruleNumber);
            } else {
                printf("%-15s", "");
            }
        }
        printf("\n");
    }
}

/**
 * Print FIRST and FOLLOW sets
 */
void printFirstAndFollow(FirstAndFollow *F) {
    printf("\n========== FIRST SETS ==========\n");
    for (int i = 0; i < F->firstCount; i++) {
        printf("FIRST(%s) = { ", getNonTerminalName(F->firstSets[i].nonTerminal));
        for (int j = 0; j < F->firstSets[i].firstCount; j++) {
            printf("%s ", getTokenName(F->firstSets[i].first[j]));
        }
        if (F->firstSets[i].hasEpsilon) {
            printf("ε ");
        }
        printf("}\n");
    }
    
    printf("\n========== FOLLOW SETS ==========\n");
    for (int i = 0; i < F->followCount; i++) {
        printf("FOLLOW(%s) = { ", getNonTerminalName(F->followSets[i].nonTerminal));
        for (int j = 0; j < F->followSets[i].followCount; j++) {
            printf("%s ", getTokenName(F->followSets[i].follow[j]));
        }
        printf("}\n");
    }
}

/**
 * Free parse tree memory
 */
void freeTreeNode(parseTreeNode *node) {
    if (!node) return;
    
    for (int i = 0; i < node->childCount; i++) {
        freeTreeNode(node->children[i]);
    }
    
    free(node);
}

void freeParseTree(parseTree *PT) {
    if (PT) {
        freeTreeNode(PT->root);
        free(PT);
    }
}

void reportSyntaxError(ParseError *error) {
    fprintf(stderr, "Line %d: %s\n", error->lineNumber, error->message);
    if (error->expectedCount > 0) {
        fprintf(stderr, "  Expected one of: ");
        for (int i = 0; i < error->expectedCount; i++) {
            fprintf(stderr, "%s ", getTokenName(error->expected[i]));
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "  Received: %s\n", getTokenName(error->received));
}