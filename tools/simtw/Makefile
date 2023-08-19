CFLAGS=-O
BIN=../../BIN
EXE=$(BIN)/simtw

all: $(EXE)

$(EXE): simtw.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(EXE) *.o
