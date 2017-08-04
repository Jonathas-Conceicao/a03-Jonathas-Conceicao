SHELL=/bin/bash

TARGET = a03-conferidor-de-magias

CC = gcc
CFLAG = -std=c99 -Wall -Wextra #-Werror # -g

# Folders
OBJ = obj
SRC = src
BIN = bin

# Colors
RED= \033[1;31m
YELLOW = \033[1;33m
GREEN = \033[1;32m
BLUE = \033[1;34m
NC = \033[0m
#
ERROR = $(RED)
WARNING = $(YELLOW)
NICE = $(GREEN)
NOTE = $(BLUE)


#################################################################################
all: grade
	@echo -e "$(NOTE)Grading$(NC)";
	./$(BIN)/$(TARGET).out

grade: version.o useful.o taa.o lfs.o block.o
#@echo -e "$(WARNIGN)Here we compile the test[?]$(NC)";
	@echo -e "$(NOTE)Compiling all together$(NC)";
	$(CC) $(CFLAG) $(OBJ)/version.o $(SRC)/test.c 	-o $(BIN)/$(TARGET).out -lm

block.o: version.o useful.o $(OBJ)/version.o $(OBJ)/useful.o $(SRC)/block.c
	@echo -e "$(NOTE)Compiling block.c$(NC)";
	$(CC) $(CFLAG) -c $(SRC)/block.c 								-o $(OBJ)/block.o

lfs.o: version.o useful.o $(OBJ)/version.o $(OBJ)/useful.o $(SRC)/lfs.c
	@echo -e "$(NOTE)Compiling lfs.c$(NC)";
	$(CC) $(CFLAG) -c $(SRC)/lfs.c 									-o $(OBJ)/lfs.o

taa.o: version.o useful.o block.o lfs.o $(OBJ)/version.o $(OBJ)/useful.o $(OBJ)/block.o $(OBJ)/lfs.o $(SRC)/taa.c
	@echo -e "$(NOTE)Compiling taa.c$(NC)";
	$(CC) $(CFLAG) -c $(SRC)/taa.c 									-o $(OBJ)/taa.o

useful.o: $(SRC)/version.h $(SRC)/useful.c
	@echo -e "$(NOTE)Compiling useful.c$(NC)";
	$(CC) $(CFLAG) -c $(SRC)/useful.c 								-o $(OBJ)/useful.o

version.o: $(SRC)/simplegrade.h $(SRC)/version.c
	@echo -e "$(NOTE)Compiling version.c$(NC)";
	$(CC) $(CFLAG) -c $(SRC)/version.c 							-o $(OBJ)/version.o -lm

testTAA:
	@echo -e "$(NOTE)Compiling taa.c$(NC)";
	$(CC) $(CFLAG) -c $(SRC)/taa.c 									-o $(OBJ)/taa.o
	@echo -e "$(NOTE)Compiling useful.c$(NC)";
	$(CC) $(CFLAG) -c $(SRC)/useful.c 								-o $(OBJ)/useful.o
	@echo -e "$(NOTE)Compiling testTAA$(NC)";
	$(CC) $(CFLAG) $(OBJ)/taa.o $(OBJ)/useful.o testTAA.c 	-o $(BIN)/a.out -lm

raiz: #Não Nutela, funciona mesmo
	$(CC) $(CFLAG) src/version.c src/useful.c src/lfs.c src/taa.c src/test.c -o $(BIN)/$(TARGET).out -lm
	./$(BIN)/$(TARGET).out

clean:
	@rm *.bin || true
	@rm $(OBJ)/*.o || true
	@rm $(BIN)/*.out || true
