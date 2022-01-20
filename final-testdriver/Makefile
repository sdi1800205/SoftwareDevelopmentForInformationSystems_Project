# Copyright (c) 2013 KAUST - InfoCloud Group (All Rights Reserved)
#
# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the "Software"), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
#
# Authors: Amin Allam  <amin.allam@kaust.edu.sa>,
#          Fuad Jamour <fuad.jamour@kaust.edu.sa>
#
# Current version: 1.0 (initial release)

# paths
INCLUDE = ../include
MODULES = ../modules
	
TEST_NEW_O=test_driver/test_through.o
IMPL_O=$(MODULES)/core.o

OBJS = $(MODULES)/ADTDeque.o $(MODULES)/ADTSet.o $(MODULES)/ADTMap.o $(MODULES)/BK_tree.o $(MODULES)/BK_List.o $(MODULES)/distances.o $(MODULES)/entry_list.o $(MODULES)/hamming.o $(MODULES)/useful_functions.o
OBJS_2 = $(MODULES)/job_scheduler.o $(MODULES)/job.o $(MODULES)/Queue.o $(MODULES)/threads.o

CC  = gcc
CXX = g++
CFLAGS=-O3 -fPIC -Wall -g -I. -I$(INCLUDE)
CXXFLAGS=$(CFLAGS)
LDFLAGS=-lpthread

PROGRAMS=new-testdriver

LIBRARY=core

all: $(PROGRAMS)

lib: $(IMPL_O)
	$(CC) $(CXXFLAGS) -shared -o lib$(LIBRARY).so $(IMPL_O)
	
new-testdriver: lib $(TEST_NEW_O) $(OBJS) $(OBJS_2)
	$(CXX) $(CXXFLAGS) $(OBJS) $(OBJS_2) -o testdriver $(TEST_NEW_O) ./lib$(LIBRARY).so $(LDFLAGS)

clean:
	rm -f testdriver lib$(LIBRARY).so result.txt
	find . -name '*.o' -print | xargs rm -f $(OBJS) $(OBJS_2) $(IMPL_O)

run: new-testdriver
	./testdriver

valgrind: new-testdriver
	valgrind --leak-check=full --show-leak-kinds=all ./testdriver