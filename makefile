CC = gcc
SRC = driver.c hashmap.c lexer.c linkedList.c parser.c parseTree.c removeComment.c ast.c symbolTable.c codegen.c
OBJ = $(SRC:.c=.o)
TARGET = compiler
GREEN = '\033[32m'
RESET = '\033[0m'
YELLOW = '\033[33m'

.PHONY: clean

$(TARGET): clean $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)
	@echo $(GREEN)"\n * Program 'compiler' compiled successfully"$(RESET)
	@echo $(GREEN)" * Welcome to SASTA (Shashank Anish Shadan Tarak Akshat) Compiler :) for ERPLAG\n"$(RESET)
	@echo $(YELLOW)" * Team Member BITS ID\t Member Name"$(RESET)
	@echo $(YELLOW)" * 2020A7PS0073P\t Shashank Agrawal"$(RESET)
	@echo $(YELLOW)" * 2020A7PS0096P\t Akshat Gupta"$(RESET)
	@echo $(YELLOW)" * 2020A7PS0129P\t Anish Ghule"$(RESET)
	@echo $(YELLOW)" * 2020A7PS0134P\t Shadan Hussain"$(RESET)
	@echo $(YELLOW)" * 2020A7PS1513P\t Tarak P V S"$(RESET)


%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)