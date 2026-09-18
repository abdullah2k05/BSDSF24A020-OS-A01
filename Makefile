# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
AR = ar
ARFLAGS = rcs

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin
LIB_DIR = lib

# Targets
STATIC_TARGET = $(BIN_DIR)/client_static
DYNAMIC_TARGET = $(BIN_DIR)/client_dynamic
STATIC_LIB = $(LIB_DIR)/libmyutils.a
DYNAMIC_LIB = $(LIB_DIR)/libmyutils.so

# Library source files
LIB_SOURCES = $(SRC_DIR)/mystrfunctions.c $(SRC_DIR)/myfilefunctions.c
LIB_OBJECTS = $(LIB_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
LIB_PIC_OBJECTS = $(LIB_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.pic.o)

# Main source and object files
MAIN_SOURCE = $(SRC_DIR)/main.c
MAIN_OBJECT = $(OBJ_DIR)/main.o

# Default rule
all: $(STATIC_TARGET) $(DYNAMIC_TARGET)

# Create static library
$(STATIC_LIB): $(LIB_OBJECTS) | $(LIB_DIR)
	$(AR) $(ARFLAGS) $@ $^
	ranlib $@

# Create dynamic library (position-independent code)
$(DYNAMIC_LIB): $(LIB_PIC_OBJECTS) | $(LIB_DIR)
	$(CC) -shared -o $@ $^

# Link main object with static library
$(STATIC_TARGET): $(MAIN_OBJECT) $(STATIC_LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJECT) -L$(LIB_DIR) -lmyutils

# Link main object with dynamic library
$(DYNAMIC_TARGET): $(MAIN_OBJECT) $(DYNAMIC_LIB) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $(MAIN_OBJECT) -L$(LIB_DIR) -lmyutils

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Compile source files into position-independent object files
$(OBJ_DIR)/%.pic.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -fPIC -c -o $@ $<

# Create output directories if they don't exist
$(BIN_DIR) $(OBJ_DIR) $(LIB_DIR):
	mkdir -p $@

# Install to system directories
PREFIX = /usr/local
install: $(STATIC_TARGET)
	install -d $(DESTDIR)$(PREFIX)/bin
	install -d $(DESTDIR)$(PREFIX)/share/man/man3
	install -m 755 $(STATIC_TARGET) $(DESTDIR)$(PREFIX)/bin/client
	install -m 644 man/man3/*.3 $(DESTDIR)$(PREFIX)/share/man/man3/

# Clean build artifacts
clean:
	rm -f $(OBJ_DIR)/*.o $(STATIC_LIB) $(DYNAMIC_LIB) $(STATIC_TARGET) $(DYNAMIC_TARGET)

# Run static program
run-static: $(STATIC_TARGET)
	./$(STATIC_TARGET)

# Run dynamic program
run-dynamic: $(DYNAMIC_TARGET)
	LD_LIBRARY_PATH=$(LIB_DIR) ./$(DYNAMIC_TARGET)

.PHONY: all clean run-static run-dynamic install
