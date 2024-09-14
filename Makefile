# CXX Make variable for compiler
CC=g++
# -std=c++11  C/C++ variant to use, e.g. C++ 2011
# -Wall       show the necessary warning files
# -g3         include information for symbolic debugger e.g. gdb 
CCFLAGS=-std=c++11 -Wall -g3 -c

# object files
OBJS = log.o fooddelivery.o consumer.o broker.o

# Program name
PROGRAM = fooddelivery

# The program depends upon its object files
$(PROGRAM) : $(OBJS)
	$(CC) -pthread -o $(PROGRAM) $(OBJS)

log.o : log.c log.h
	$(CC) $(CCFLAGS) log.c
	
fooddelivery.o : fooddelivery.cpp fooddelivery.h
	$(CC) $(CCFLAGS) fooddelivery.cpp

broker.o : broker.cpp broker.h
	$(CC) $(CCFLAGS) broker.cpp

consumer.o : consumer.cpp consumer.h
	$(CC) $(CCFLAGS) consumer.cpp


# Once things work, people frequently delete their object files.
# If you use "make clean", this will do it for you.
# As we use gnuemacs which leaves auto save files termintating
# with ~, we will delete those as well.
clean :
	rm -f *.o *~ $(PROGRAM)

