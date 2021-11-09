CC=gcc
CFLAGS=-I.
DEPS = interface.h core.h
OBJ = main.o entry_list.o
EXEC = main
ARGS = 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~ *.exe

run: $(EXEC)
	./$(EXEC) $(ARGS)