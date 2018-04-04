PROG = sadgb
CC = g++
BIN = bin/
SRC = src/
INC = include/
CPPFLAGS = -std=c++11 -g -Wall -I $(INC)
LDFLAGS = -lncursesw $(INC)*.cpp
FILES = main CPU MMU TextGPU Cartridge InterruptService
OBJS = $(addprefix $(BIN), $(FILES:=.o))


$(PROG) : $(FILES)
	$(CC) $(LDFLAGS) -std=c++11 -o $(BIN)$(PROG) $(OBJS)
	make clean
	#printf "\033c" #clear screen

main:
	$(CC) $(CPPFLAGS) -c $(SRC)$@.cpp -o $(BIN)$@.o
	
CPU: $(SRC)CPU.h 
	$(CC) $(CPPFLAGS) -c $(SRC)$@.cpp -o $(BIN)$@.o
	
MMU: $(SRC)MMU.h
	$(CC) $(CPPFLAGS) -c $(SRC)$@.cpp -o $(BIN)$@.o
	
Cartridge: $(SRC)Cartridge.h
	$(CC) $(CPPFLAGS) -c $(SRC)$@.cpp -o $(BIN)$@.o
	
TextGPU: $(SRC)Graphics/TextGPU.h
	$(CC) $(CPPFLAGS) -c $(SRC)Graphics/$@.cpp -o $(BIN)$@.o -l $(SRC)Graphics/iGPU.h  -l $(SRC)MMU.h
	
InterruptService: $(SRC)InterruptService.h
	$(CC) $(CPPFLAGS) -c $(SRC)$@.cpp -o $(BIN)$@.o

clean:
	rm -f $(BIN)*.o
	
reset: clean
	rm -f $(BIN)$(PROG)