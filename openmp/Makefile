EXE = knight_tour_seq.out

SRC_DIR = src
OBJ_DIR = obj
INCLUDE_DIR = include

SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

CC = gcc

CPPFLAGS += -I$(INCLUDE_DIR)
CFLAGS += -std=c99 -Wall -Werror -fopenmp

.PHONY: all clean

all: clean $(EXE)

$(EXE): $(OBJ)
	$(CC) $^ -lgomp -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)
	$(RM) $(EXE)

run: all
	./$(EXE) 8 8 1 1 knight_tour_seq.txt
