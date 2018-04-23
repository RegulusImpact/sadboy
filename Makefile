PROG = sadboy
CC = g++
BIN = bin/
SRC = src/
INC = include/
CPPFLAGS = -std=c++14 -g -Wall -Wextra -pedantic -I $(INC)
LDFLAGS =  $(INC)*.cpp #-lncursesw
FILES = main CPU MMU Cartridge InterruptService TimerService XGPU MBC
OBJS = $(addprefix $(BIN), $(FILES:=.o))


$(PROG) : $(FILES)
	$(CC) $(LDFLAGS) -std=c++11 -o $(BIN)$(PROG) $(OBJS) -lX11
	make clean

main:
	$(CC) $(CPPFLAGS) -c $(SRC)$@.cpp -o $(BIN)$@.o

CPU: $(SRC)CPU.h
	$(CC) $(CPPFLAGS) -c $(SRC)$@.cpp -o $(BIN)$@.o

MMU: $(SRC)MMU.h
	$(CC) $(CPPFLAGS) -c $(SRC)$@.cpp -o $(BIN)$@.o

Cartridge: $(SRC)Cartridge.h
	$(CC) $(CPPFLAGS) -c $(SRC)$@.cpp -o $(BIN)$@.o

MBC: $(SRC)MBC.h
	$(CC) $(CPPFLAGS) -c $(SRC)$@.cpp -o $(BIN)$@.o

# TextGPU: $(SRC)Graphics/TextGPU.h
	# $(CC) $(CPPFLAGS) -c $(SRC)Graphics/$@.cpp -o $(BIN)$@.o -l $(SRC)Graphics/iGPU.h  -l $(SRC)MMU.h

XGPU: $(SRC)Graphics/XGPU.h
	$(CC) $(CPPFLAGS) -c $(SRC)Graphics/$@.cpp -o $(BIN)$@.o -l $(SRC)Graphics/iGPU.h -lX11

InterruptService: $(SRC)InterruptService.h
	$(CC) $(CPPFLAGS) -c $(SRC)$@.cpp -o $(BIN)$@.o

TimerService: $(SRC)TimerService.h
	$(CC) $(CPPFLAGS) -c $(SRC)$@.cpp -o $(BIN)$@.o

clean:
	rm -f $(BIN)*.o

reset: clean
	rm -f $(BIN)$(PROG)
