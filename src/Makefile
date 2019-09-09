CC = gcc
SRC = main.c
EXE = myls

all: compile
	@echo Done!
	@echo
	@echo Usage: ./$(EXE) \< -Ulsa \>

compile:
	@echo Compiling...
	@$(CC) $(SRC) -o $(EXE)

clean:
	@echo Cleaning up..
	@rm -f *.o
	@rm -f *.core
	@rm -f *.out
	@rm -f $(EXE)

