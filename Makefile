# Compiler: gcc
CC = gcc

# Compiler flags & libraries
IDIR = -I./inc \
	-I./inc/war \
	-I./inc/solitaire
CFLAGS = -O3 -std=c99 $(IDIR)

# Directories
# BIN_DIR = bin
SDIRS = war solitaire
SRC = src
SRC_DIR = $(SRC) $(addprefix $(SRC)/, $(SDIRS))
SRC_FILES = $(notdir $(foreach sdir, $(SRC_DIR), $(wildcard $(sdir)/*.c)))
OBJ_DIR = src
#obj
# Executable, objects, source
NAME = CardsSuite
# $(BIN_DIR)/main
OBJS = $(patsubst %.c, %.o,$(SRC_FILES))
VPATH = $(SRC_DIR)
.PHONY : all clean

all : $(NAME)

$(NAME) : $(addprefix $(OBJ_DIR)/, $(OBJS))
	$(CC) $(CFLAGS) -o $@ $^

$(OBJ_DIR)/%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $^

clean :
	-rm $(NAME) $(OBJ_DIR)/*.o
