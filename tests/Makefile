# paths
INCLUDE = ../include
MODULES = ../modules

# compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall -Werror -g -I$(INCLUDE)

# Αρχεία .o
OBJS = entry_test.o $(MODULES)/BK_tree.o $(MODULES)/BK_List.o $(MODULES)/entry_list.o $(MODULES)/distances.o $(MODULES)/ADTSet.o $(MODULES)/useful_functions.o

# Το εκτελέσιμο πρόγραμμα
EXEC = entry_test

# Παράμετροι για δοκιμαστική εκτέλεση
ARGS =

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

clean:
	rm -f $(OBJS) $(EXEC)

run: $(EXEC)
	./$(EXEC) $(ARGS)