# paths
INCLUDE = ../include
MODULES = ../modules

# compiler
CC = gcc

# Compile options. Το -I<dir> λέει στον compiler να αναζητήσει εκεί include files
CFLAGS = -Wall -Werror -g -I$(INCLUDE)

# Αρχεία .o
OBJS = entry_test.o $(MODULES)/BK_tree.o $(MODULES)/BK_List.o $(MODULES)/entry_list.o $(MODULES)/distances.o $(MODULES)/ADTSet.o $(MODULES)/useful_functions.o
OBJS_2 = ADTDeque_test.o $(MODULES)/ADTDeque.o $(MODULES)/useful_functions.o $(MODULES)/distances.o
OBJS_3 = ADTMap_test.o $(MODULES)/ADTMap.o $(MODULES)/ADTSet.o $(MODULES)/useful_functions.o $(MODULES)/distances.o

# Το εκτελέσιμο πρόγραμμα
EXEC = entry_test
EXEC_2 = ADTDeque_test
EXEC_3 = ADTMap_test

# Παράμετροι για δοκιμαστική εκτέλεση
ARGS =

all: $(OBJS) $(OBJS_2) $(OBJS_3)
	$(CC) $(OBJS) -o $(EXEC)
	$(CC) $(OBJS_2) -o $(EXEC_2)
	$(CC) $(OBJS_3) -o $(EXEC_3)

entry_test: $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

ADTDeque_test: $(OBJS_2)
	$(CC) $(OBJS_2) -o $(EXEC_2)

ADTMap_test: $(OBJS_3)
	$(CC) $(OBJS_3) -o $(EXEC_3)

clean:
	rm -f $(OBJS) $(OBJS_2) $(OBJS_3) $(EXEC) $(EXEC_2) $(EXEC_3)

run: $(EXEC) $(EXEC_2) $(EXEC_3)
	./$(EXEC) $(ARGS)
	./$(EXEC_2) $(ARGS)
	./$(EXEC_3) $(ARGS)