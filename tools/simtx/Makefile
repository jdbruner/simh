CFLAGS=-O
BIN=../../BIN
EXE=$(BIN)/simtx

all: $(EXE)

$(EXE): simtx.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm $(EXE) *.o
