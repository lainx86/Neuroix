# =================================================================
#  NEUROIX MAKEFILE (Final Integration)
# =================================================================

# 1. Compiler & Flags
CC      = gcc
# Penting: Tambahkan -fopenmp karena llama.cpp menggunakan multithreading
CFLAGS  = -Wall -Wextra -O3 -g -std=c11 -fopenmp

# 2. Directories
SRC_DIR   = src
INC_DIR   = include
BUILD_DIR = build
BIN_DIR   = bin
LIB_DIR   = lib

# 3. Llama.cpp Integration
LLAMA_ROOT = $(LIB_DIR)/llama.cpp
LLAMA_BUILD = $(LLAMA_ROOT)/build

# INCLUDE PATHS:
# Kita butuh header dari folder include, common, dan ggml/include
LLAMA_INC  = -I$(LLAMA_ROOT)/include \
             -I$(LLAMA_ROOT)/common \
             -I$(LLAMA_ROOT)/ggml/include

# LIBRARY PATHS (-L):
# Memberitahu gcc di mana mencari file .a yang kamu temukan tadi
LLAMA_LIB_DIRS = -L$(LLAMA_BUILD)/src \
                 -L$(LLAMA_BUILD)/ggml/src

# LINK LIBRARIES (-l):
# Urutan linking itu penting di GCC!
# 1. llama (Level tertinggi)
# 2. ggml-* (Dependency llama)
# 3. stdc++ (Karena library aslinya C++)
# 4. m (Math library)
LLAMA_LIBS = $(LLAMA_LIB_DIRS) \
             -lllama \
             -lggml \
             -lggml-cpu \
             -lggml-base \
             -lstdc++ \
             -lm \
             -fopenmp

# Gabungkan Include Path
INCLUDES = -I$(INC_DIR) $(LLAMA_INC)

# 4. Files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))

# Output Binary Name
TARGET = $(BIN_DIR)/neuroix

# =================================================================
#  Build Rules
# =================================================================

all: check_dirs $(TARGET)
	@echo "[SUCCESS] Neuroix Kernel built: $(TARGET)"

# Linking Step
$(TARGET): $(OBJS)
	@echo "[LINK] Linking object files..."
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(LLAMA_LIBS)

# Compilation Step
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "[CC] Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

check_dirs:
	@mkdir -p $(BIN_DIR) $(BUILD_DIR)

clean:
	@echo "[CLEAN] Removing build artifacts..."
	@rm -rf $(BIN_DIR)/* $(BUILD_DIR)/*

run: all
	@echo "[RUN] Starting Neuroix Kernel..."
	@./$(TARGET)

.PHONY: all clean run check_dirs