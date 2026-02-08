# Makefile for Compiler Front-End
# CS F363 Compiler Design Project
# Compatible with GCC 13.3.0 and Ubuntu 24.04.2 LTS

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = -lm

# Target executable
TARGET = stage1exe

# Source files
SOURCES = driver.c lexer.c parser.c
OBJECTS = $(SOURCES:.c=.o)

# Header dependencies
HEADERS = lexerDef.h lexer.h parserDef.h parser.h

# Default target
all: $(TARGET)
	@echo "Build successful! Executable: $(TARGET)"
	@echo "Usage: ./$(TARGET) <source_file.txt> <parsetree_output.txt>"

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files to object files
driver.o: driver.c lexer.h parser.h
	$(CC) $(CFLAGS) -c $<

lexer.o: lexer.c lexer.h lexerDef.h
	$(CC) $(CFLAGS) -c $<

parser.o: parser.c parser.h parserDef.h lexer.h lexerDef.h
	$(CC) $(CFLAGS) -c $<

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET) clean_code.txt
	@echo "Clean complete"

# Clean and rebuild
rebuild: clean all

# Run with a test file
test: $(TARGET)
	./$(TARGET) testcase.txt parsetree.txt

# Help target
help:
	@echo "Makefile for Compiler Front-End"
	@echo ""
	@echo "Targets:"
	@echo "  all      - Build the compiler (default)"
	@echo "  clean    - Remove object files and executable"
	@echo "  rebuild  - Clean and rebuild"
	@echo "  test     - Build and run with testcase.txt"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make                           # Build compiler"
	@echo "  make clean                     # Clean build"
	@echo "  ./stage1exe input.txt out.txt  # Run compiler"

.PHONY: all clean rebuild test help
