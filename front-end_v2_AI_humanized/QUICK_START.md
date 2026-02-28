# Quick Start

```bash
make                        # build stage1exe
make test_lexer             # build standalone lexer tester
./stage1exe t3.txt out.txt  # run compiler (menu-driven)
./test_lexer t1.txt         # run lexer only
```

Menu options: `1` = strip comments, `2` = print tokens, `3` = parse + tree, `4` = timing, `0` = exit.

Run parser tests:
```bash
make test-parser   # runs t3, t4, t5, t6
make test-lexer    # runs t1, t2
```

For troubleshooting, try the lexer standalone first (`./test_lexer`) before running the full parser.
