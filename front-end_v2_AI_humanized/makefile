# Makefile - compiler front-end (CS F363)
# Works with GCC/Clang on macOS and Linux

CC     = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -lm

TARGET     = stage1exe
TEST_LEXER = test_lexer

SOURCES = driver.c lexer.c parser.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = lexerDef.h lexer.h parserDef.h parser.h

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_LEXER): test_lexer.o lexer.o
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

driver.o: driver.c lexer.h parser.h
	$(CC) $(CFLAGS) -c $<

lexer.o: lexer.c lexer.h lexerDef.h
	$(CC) $(CFLAGS) -c $<

parser.o: parser.c parser.h parserDef.h lexer.h lexerDef.h
	$(CC) $(CFLAGS) -c $<

test_lexer.o: test_lexer.c lexer.h lexerDef.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS) test_lexer.o $(TARGET) $(TEST_LEXER) clean_code.txt

rebuild: clean all

# quick sanity check
test: $(TARGET)
	./$(TARGET) testcase.txt parsetree.txt

test-lexer: $(TEST_LEXER)
	@echo ""
	@echo "=== Lexer Test: t1.txt ==="
	./$(TEST_LEXER) lexer_test_cases/t1.txt
	@echo ""
	@echo "=== Lexer Test: t2.txt ==="
	./$(TEST_LEXER) lexer_test_cases/t2.txt

test-parser: $(TARGET)
	@echo ""
	@echo "=== Parser Test: t3.txt ==="
	@echo "3" | ./$(TARGET) parser_test_cases/t3.txt parsetree_t3.txt
	@echo ""
	@echo "=== Parser Test: t4.txt ==="
	@echo "3" | ./$(TARGET) parser_test_cases/t4.txt parsetree_t4.txt
	@echo ""
	@echo "=== Parser Test: t5.txt ==="
	@echo "3" | ./$(TARGET) parser_test_cases/t5.txt parsetree_t5.txt
	@echo ""
	@echo "=== Parser Test: t6.txt (expects errors) ==="
	@echo "3" | ./$(TARGET) parser_test_cases/t6.txt parsetree_t6.txt

.PHONY: all clean rebuild test test-lexer test-parser
