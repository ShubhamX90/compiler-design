# QUICK START GUIDE
## Compiler Front-End - CS F363

### 🚀 Get Started in 3 Steps

#### Step 1: Build the Compiler
```bash
make
```

#### Step 2: Run a Test
```bash
./stage1exe testcase1.txt parsetree.txt
```

#### Step 3: Test the Lexer (Standalone)
```bash
gcc -o test_lexer test_lexer.c lexer.c -I. -Wall
./test_lexer testcase1.txt
```

---

## Quick Command Reference

### Build Commands
```bash
make              # Build compiler
make clean        # Remove build artifacts
make rebuild      # Clean and rebuild
```

### Run Commands
```bash
# Interactive menu mode
./stage1exe testcase1.txt output.txt

# Test just the lexer
./test_lexer testcase1.txt
```

---

## Menu Options Quick Reference

When you run `./stage1exe`, you'll see:

```
0 : Exit
1 : Remove comments and print clean code
2 : Print token list (lexical analysis)
3 : Parse and verify syntax (with parse tree)
4 : Print total time taken
```

**Most Common Usage**: Option 3 (full parsing with parse tree generation)

---

## Test Files Provided

1. **testcase1.txt** - Simple function with arithmetic
2. **testcase2.txt** - Conditionals and loops  
3. **testcase3.txt** - Record types
4. **testcase_errors.txt** - Error handling demonstration

---

## Common Tasks

### Task: Check if source code has lexical errors
```bash
./test_lexer your_file.txt
```

### Task: Get complete parse tree
```bash
./stage1exe your_file.txt output_parsetree.txt
# Then select option 3
```

### Task: See all tokens
```bash
./test_lexer your_file.txt | less
```

### Task: Test with your own code
1. Create a file following language spec (see testcase*.txt examples)
2. Run: `./stage1exe yourcode.txt output.txt`
3. Select option 3

---

## Language Quick Reference

### Valid Identifiers
```
Variables:  b2, c3d7, d2bbb5555
Functions:  _main, _function1, _myFunc
Records:    #book, #student
Fields:     price, edition, name
```

### Basic Program Structure
```
_functionName
input parameter list [int b2, int c3]
output parameter list [int d5];
type int: b4;
% Your code here
return [d5];
end

_main
% Main program code
return;
end
```

### Operators
```
Assignment:    <---
Arithmetic:    +  -  *  /
Relational:    <  <=  ==  >  >=  !=
Logical:       &&&  @@@  ~
```

### Comments
```
% This is a comment
```

---

## Troubleshooting

### "Cannot open file"
- Check file exists: `ls -l yourfile.txt`
- Check file path is correct

### "Lexical error"
- Check token patterns match spec
- Common errors: `<-` (should be `<---`), `23.4` (should be `23.40`)

### "Syntax error"
- Check semicolons after statements
- Check `end` keyword after functions
- Check parentheses/brackets match

### Build fails
```bash
make clean
make rebuild
```

---

## File Organization

```
Your Project Directory/
├── stage1exe              ← Main compiler executable
├── test_lexer             ← Standalone lexer tester
├── testcase1.txt          ← Example: simple function
├── testcase2.txt          ← Example: control flow
├── testcase3.txt          ← Example: records
├── README.md              ← Full documentation
└── QUICK_START.md         ← This file
```

---

## Getting Help

1. **For language syntax**: Check testcase*.txt examples
2. **For compilation**: Check README.md "Building" section
3. **For errors**: Use test_lexer first to check tokens
4. **For parser**: Check grammar rules in README.md

---

## Pro Tips

✅ **Always test lexer first** with `test_lexer` before full parsing
✅ **Check line numbers** in error messages to locate issues
✅ **Compare with testcases** when writing your own code
✅ **Use comments** liberally (lines starting with %)
✅ **Build clean** before submission: `make clean && make`

---

## Next Steps

1. ✅ Build: `make`
2. ✅ Test provided cases: `./test_lexer testcase1.txt`
3. ✅ Write your own test case
4. ✅ Full parse: `./stage1exe your_test.txt output.txt` → Option 3
5. ✅ Review parse tree in output.txt

---

## Important Notes for Submission

- Make sure `makefile` builds cleanly with no errors
- Test with all provided testcases
- Verify parse tree output format is correct
- Document any special cases in comments
- Keep README.md updated with team info

---

## Success Checklist

Before considering your project complete:

- [ ] Code compiles without errors: `make`
- [ ] Lexer recognizes all tokens: `./test_lexer testcase1.txt`
- [ ] Parser accepts valid programs: Option 3 on testcase1.txt
- [ ] Error detection works: Test with testcase_errors.txt
- [ ] Parse tree format is correct: Check output.txt
- [ ] Timing works: Option 4
- [ ] All team members understand the code
- [ ] Documentation is complete

---

**🎉 You're ready to go! Start with `make` and then `./test_lexer testcase1.txt`**

For detailed documentation, see **README.md**
