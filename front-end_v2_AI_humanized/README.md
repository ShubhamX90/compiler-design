# Compiler Front-End
## CS F363 Compiler Design — BITS Pilani

---

## What this is

A front-end compiler for a custom language defined in CS F363. It has two main parts:

1. **Lexer** — tokenizes source code with a DFA and twin-buffer I/O
2. **Parser** — LL(1) predictive parser that builds a parse tree

Both modules are in C and compile with GCC or Clang on macOS/Linux.

---

## Files

```
lexerDef.h          token types, DFA states, buffer struct
lexer.h / lexer.c   lexer implementation
parserDef.h         grammar/parse tree data structures  
parser.h / parser.c parser (FIRST/FOLLOW, parse table, tree)
driver.c            interactive menu that ties everything together
test_lexer.c        standalone lexer test harness
makefile
Modified LL(1) Grammar.pdf
lexer_test_cases/   t1.txt, t2.txt + expected token files
parser_test_cases/  t3-t6.txt + listoferrors_t6.txt
```

---

## Building

```bash
make              # builds stage1exe
make test_lexer   # builds standalone lexer tester
make clean        # remove build artifacts
make rebuild      # clean + build
```

---

## Running

```bash
./stage1exe <source.txt> <parsetree_output.txt>
```

Then pick from the menu:

```
0 : exit
1 : remove comments, print clean source
2 : print all tokens (lexical analysis)
3 : parse and write parse tree to file
4 : time the whole thing
```

The standalone lexer:
```bash
./test_lexer lexer_test_cases/t1.txt
```

---

## Identifiers & tokens

| Category | Pattern | Example |
|---|---|---|
| Variable | `[b-d][2-7][b-d]*[2-7]*` | `b2`, `d5cc34` |
| Function | `_[a-zA-Z]+[0-9]*` | `_main`, `_foo` |
| Record/Union ID | `#[a-z]+` | `#student` |
| Field | `[a-z]+` | `price` |
| Integer | `[0-9]+` | `123` |
| Real | `[0-9]+.[0-9][0-9]` | `12.34`, `1.23E+01` |
| Assignment | `<---` | |
| AND / OR / NOT | `&&&` / `@@@` / `~` | |

Comments start with `%` and go to end of line.

---

## Error handling

Lexical errors (unknown symbols, bad number patterns, ids too long) go to stderr with line numbers and appear in the token listing.

Syntax errors are reporterd through panic-mode recovery — bad tokens are skipped until a sync token (`;`, `end`, `endif`, etc.) is found.

---

## Known quirks

- `<-` is tokenized as `TK_LT` + `TK_MINUS` (double retract)
- `23.abc` → `"23."` is TK_ERROR, then `"abc"` is TK_FIELDID on the next call
- While/if bodies use `<stmt><otherStmts>` not `<stmts>` because `<stmts>` mandates a return statement which can't appear mid-block
- Max var id length: 20 chars; max fun id length: 30 chars
