CC = gcc
LDFLAGS = -lgmp -lmpfr -lpng -fopenmp -g
CFLAGS = -W -Wall -Wextra -O3 $(LDFLAGS)
#
# SRC extension
#
EXT = c
#
# Directories.
#
OBJ_REP = obj
INC_REP = inc
SRC_REP = src
BIN_REP = bin
#
# Sources, objects and binary file name.
#
SRC = $(foreach dir, $(SRC_REP), $(wildcard $(dir)/*.$(EXT)))
OBJ = $(addsuffix .o, $(basename $(subst ${SRC_REP}, ${OBJ_REP}, ${SRC})))
BIN = main

UNAME = $(shell uname)
ifeq ($(UNAME), Darwin)
CC = gcc-5
endif

.PHONY: clean mrproper clear dir
.SUFFIXES:

all: dir $(BIN_REP)/$(BIN)
#
# BIN creation.
#
$(BIN_REP)/$(BIN): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@ -I$(INC_REP)
	@echo -e "> ./"$(BIN_REP)"/"$(BIN)" well created !\n"
#
# Objects files creation.
#
$(OBJ_REP)/%.o: $(SRC_REP)/%.$(EXT)
	$(CC) $(CFLAGS) -c $^ -o $@ -I$(INC_REP)
	@echo -e "> ./"$@" well created !\n"
#
# Other usefull targets.
#
clean:
	rm -f ./$(OBJ_REP)/*.o;
	rm -f output

mrproper:
	rm -f ./$(OBJ_REP)/*.o ./$(BIN_REP)/$(BIN);
	rm -f output

rebuild: mrproper clear all

#
# Screen cleaning.
#
clear:
	clear
#
# Directories creation.
#
dir:
	@mkdir -p $(OBJ_REP)
	@mkdir -p $(BIN_REP)
