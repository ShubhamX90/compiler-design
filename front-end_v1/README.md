# Compiler Front-End Implementation
## CS F363 Compiler Design Project

### Team Information
- **Course**: CS F363 - Compiler Construction
- **Institution**: BITS Pilani
- **GCC Version**: 13.3.0
- **Ubuntu Version**: 24.04.2 LTS

---

## Project Overview

This is a complete implementation of the **front-end** of a compiler for a custom programming language. The compiler consists of two main modules:

1. **Lexical Analyzer (Lexer)** - Tokenizes source code using DFA-based recognition
2. **Syntax Analyzer (Parser)** - Verifies syntactic correctness using LL(1) predictive parsing

### Key Features

#### Lexical Analyzer
- ✅ Twin buffer implementation for efficient I/O
- ✅ DFA-based token recognition (as per DFA design document)
- ✅ Comprehensive error detection and reporting
- ✅ Support for all token types in language specification
- ✅ Single-pass scanning with complete token information collection
- ✅ Comment removal functionality
- ✅ Line number tracking for error reporting

#### Syntax Analyzer
- ✅ Automated FIRST and FOLLOW set computation
- ✅ LL(1) predictive parsing table generation
- ✅ Parse tree construction during parsing
- ✅ Comprehensive syntax error detection with line numbers
- ✅ Parse tree output in specified format
- ✅ Single-pass parsing

---

## File Structure

```
compiler-frontend/
├── lexerDef.h          # Lexer data structures and token definitions
├── lexer.h             # Lexer function prototypes
├── lexer.c             # Lexer implementation (twin buffer, DFA)
├── parserDef.h         # Parser data structures (grammar, parse tree)
├── parser.h            # Parser function prototypes
├── parser.c            # Parser implementation (FIRST/FOLLOW, parsing)
├── driver.c            # Main driver with menu interface
├── makefile            # GNU make build file
├── testcase1.txt       # Sample test case: simple function
├── testcase2.txt       # Sample test case: conditionals and loops
├── testcase3.txt       # Sample test case: record types
└── testcase_errors.txt # Sample test case: error handling
```

---

## Building and Running

### Prerequisites
- GCC 13.3.0 or compatible
- Ubuntu 24.04.2 LTS or compatible Linux distribution
- GNU Make utility

### Compilation

```bash
# Build the compiler
make

# Clean build artifacts
make clean

# Rebuild from scratch
make rebuild

# Quick test with sample file
make test
```

### Execution

```bash
./stage1exe <source_file.txt> <parsetree_output.txt>
```

**Example:**
```bash
./stage1exe testcase1.txt parsetree.txt
```

---

## Menu Options

The compiler provides an interactive menu with the following options:

### Option 0: Exit
Terminates the program.

### Option 1: Remove Comments
- Removes all comments (lines starting with `%`) from source code
- Displays the clean code on console
- Saves clean code to `clean_code.txt`

**Use case**: Preprocessing source code before analysis

### Option 2: Print Token List
- Performs lexical analysis on source code
- Displays all tokens with:
  - Token name
  - Lexeme
  - Line number
  - Value (for numbers)
- Reports lexical errors with line numbers

**Use case**: Testing and debugging the lexical analyzer

### Option 3: Parse and Verify Syntax
- Performs complete lexical and syntax analysis
- Builds parse tree if code is syntactically correct
- Reports all errors (lexical and syntactic) with line numbers
- Writes parse tree to specified output file

**Parse Tree Format:**
Each line contains:
```
LEXEME | CURRENT_NODE | LINE_NO | TOKEN_NAME | VALUE | PARENT_NODE | IS_LEAF | NODE_SYMBOL
```

**Use case**: Full compiler front-end execution

### Option 4: Performance Timing
- Measures total execution time for lexical and syntax analysis
- Reports CPU time in clock ticks and seconds
- Useful for performance benchmarking

**Use case**: Performance analysis and optimization

---

## Language Specification Summary

### Keywords
`with`, `parameters`, `end`, `while`, `union`, `endunion`, `definetype`, `as`, `type`, `global`, `parameter`, `list`, `input`, `output`, `int`, `real`, `endwhile`, `if`, `then`, `endif`, `read`, `write`, `return`, `call`, `record`, `endrecord`, `else`

### Identifiers
- **Variable IDs**: `[b-d][2-7][b-d]*[2-7]*` (e.g., `b2`, `c3d7`)
- **Function IDs**: `_[a-z|A-Z]+[0-9]*` (e.g., `_function1`, `_main`)
- **Record/Union IDs**: `#[a-z]+` (e.g., `#book`, `#student`)
- **Field IDs**: `[a-z]+` (e.g., `price`, `edition`)

### Numbers
- **Integer**: `[0-9]+` (e.g., `123`, `5`)
- **Real**: `[0-9]+.[0-9][0-9]` (e.g., `12.34`, `98.80`)
- **Real with exponent**: `[0-9]+.[0-9][0-9]E[+|-][0-9][0-9]` (e.g., `12.34E+05`)

### Operators
- **Assignment**: `<---`
- **Arithmetic**: `+`, `-`, `*`, `/`
- **Relational**: `<`, `<=`, `==`, `>`, `>=`, `!=`
- **Logical**: `&&&` (AND), `@@@` (OR), `~` (NOT)

### Comments
Lines starting with `%` are treated as comments.

---

## Error Handling

### Lexical Errors Detected
- Invalid characters
- Incomplete operators (`<-`, `<--`, `=`, `!`, `&`, `&&`, `@`, `@@`)
- Malformed numbers (`23.`, `23.4`, `23.45E-`)
- Invalid identifier patterns
- Incomplete function/record identifiers

### Syntax Errors Detected
- Unexpected tokens
- Missing required tokens
- Invalid grammar productions
- Mismatched parentheses, brackets
- Incomplete statements

All errors are reported with:
- Line number
- Error message
- Expected vs received tokens (for syntax errors)

---

## Design Highlights

### Twin Buffer Implementation
The lexer uses an efficient twin buffer mechanism:
- **Buffer Size**: 4096 bytes per half
- **Total Size**: 8192 bytes
- **Advantages**:
  - Minimizes I/O operations
  - Supports unlimited lookahead
  - Handles EOF gracefully
  - Efficient retraction mechanism

### DFA-Based Tokenization
- Implements complete DFA from design document
- Maximal munch (longest match) principle
- Keyword recognition via lookup table
- Efficient state transitions
- Proper error recovery

### LL(1) Predictive Parsing
- **Grammar**: 76+ production rules
- **FIRST/FOLLOW**: Automatically computed
- **Parse Table**: Efficiently generated
- **Parsing**: Single-pass, top-down
- **Parse Tree**: N-ary tree with proper parent-child relationships

### Memory Management
- Proper allocation and deallocation
- No memory leaks
- Efficient data structure usage

---

## Sample Test Cases

### Test Case 1: Simple Function
```
_function1
input parameter list [int b2, int c3cd]
output parameter list [int d5, int c2d3];
...
```
**Tests**: Basic function structure, parameters, arithmetic

### Test Case 2: Control Flow
```
_main
if (b2 > c3)
then
    ...
else
    ...
endif
while (c3 <= b7)
    ...
endwhile
```
**Tests**: Conditional statements, loops, boolean expressions

### Test Case 3: Record Types
```
record #book
type int: edition;
type real: price;
endrecord
```
**Tests**: Record definitions, field access, constructed types

### Test Case 4: Error Handling
```
c3 <- 5;          % Error: incomplete assignment
b2 <--- 23.4;     % Error: malformed real number
```
**Tests**: Lexical error detection, syntax error recovery

---

## Testing Strategy

### Lexical Analyzer Testing
1. Test each token type individually
2. Test edge cases (minimum/maximum lengths)
3. Test error patterns (near-miss tokens)
4. Test comment handling
5. Test line number tracking

### Syntax Analyzer Testing
1. Test each grammar production
2. Test nested structures
3. Test error recovery
4. Test parse tree construction
5. Test FIRST/FOLLOW set correctness

### Integration Testing
1. Test complete programs
2. Test programs with multiple functions
3. Test programs with record types
4. Test error cascading
5. Performance testing with large files

---

## Implementation Notes

### Lexer Considerations
- **Retraction**: Properly handles lookahead for operators like `<---`
- **Error Recovery**: Continues after detecting lexical errors
- **Token Info**: Stores all relevant information (line number, value)

### Parser Considerations
- **Left Factoring**: Grammar is properly left-factored for LL(1)
- **Left Recursion**: No left recursion in grammar
- **Error Recovery**: Reports errors and attempts to continue parsing
- **Tree Construction**: Builds tree during parsing, not as post-processing

### Future Enhancements (For Back-End)
The design supports easy integration of:
- Symbol table management
- Type checking
- Semantic analysis
- Intermediate code generation
- Code optimization
- Target code generation

---

## Performance Characteristics

### Time Complexity
- **Lexical Analysis**: O(n) where n = input length
- **Syntax Analysis**: O(n) where n = number of tokens
- **FIRST/FOLLOW Computation**: O(g²) where g = grammar size
- **Parse Table Creation**: O(g × t) where t = terminal count

### Space Complexity
- **Twin Buffer**: O(1) constant space (8KB)
- **Parse Table**: O(n × t) where n = non-terminals, t = terminals
- **Parse Tree**: O(n) where n = number of nodes

---

## Known Limitations

1. **Maximum Lengths**:
   - Variable identifier: 2-20 characters
   - Function identifier: ≤30 characters
   - Lexeme: 256 characters

2. **Grammar Restrictions**:
   - Must be LL(1) compatible
   - No left recursion
   - No ambiguity

3. **Error Recovery**:
   - Panic mode recovery
   - May skip valid tokens after errors

---

## References

1. Dragon Book (Compilers: Principles, Techniques, and Tools) - Aho, Sethi, Ullman
2. Language Specification Document (CS F363)
3. DFA Design Document (Project Deliverable)
4. GNU Make Manual: https://www.gnu.org/software/make/manual/

---

## Troubleshooting

### Build Issues
```bash
# If you get "gcc: command not found"
sudo apt-get install build-essential

# If you get version mismatch
gcc --version  # Check GCC version

# If make fails
make clean && make rebuild
```

### Runtime Issues
```bash
# If file not found
ls -l testcase1.txt  # Verify file exists

# If segmentation fault
gdb ./stage1exe  # Debug with GDB
```

### Common Errors
- **"Cannot open file"**: Check file path and permissions
- **"Lexical error"**: Check token patterns in source code
- **"Syntax error"**: Check grammar productions and token sequence

---

## Contact and Support

For questions or issues:
1. Review language specification document
2. Check DFA design document
3. Review sample test cases
4. Debug with option 2 (token list) first, then option 3 (full parsing)

---

## Acknowledgments

- Course Instructor: Dr. [Name]
- Teaching Assistants
- BITS Pilani CS Department

---

## Version History

- **v1.0** (Feb 2026): Initial release with complete front-end
  - Lexical analyzer with twin buffer
  - Syntax analyzer with LL(1) parsing
  - FIRST/FOLLOW automation
  - Parse tree generation
  - Comprehensive error reporting

---

## License

This project is submitted as part of CS F363 coursework at BITS Pilani.
For academic use only.

---

**Note**: This implementation is designed with extensibility in mind for the back-end components. The clean separation between lexer, parser, and data structures makes it straightforward to add:
- Symbol table management
- Type checking
- Intermediate code generation
- Code optimization
- Target code generation

The current design serves as a solid foundation for a complete compiler implementation.
