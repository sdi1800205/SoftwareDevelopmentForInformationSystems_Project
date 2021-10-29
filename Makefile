CC=gcc
CFLAGS=-I.
DEPS = entry_list.h core.h
OBJ = main.o entry_list.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

main: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~ 