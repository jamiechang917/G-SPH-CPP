OBJ = main.o myvector.o particle.o tree.o force.o updater.o fileIO.o
CC = g++
CFLAGS = -Wall -Wextra -O2 -g
LFLAGS = -lm

program: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o program  $(LFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

myvector.o: myvector.cpp
	$(CC) $(CFLAGS) -c myvector.cpp

particle.o: particle.cpp
	$(CC) $(CFLAGS) -c particle.cpp

tree.o: tree.cpp
	$(CC) $(CFLAGS) -c tree.cpp

force.o: force.cpp
	$(CC) $(CFLAGS) -c force.cpp

updater.o: updater.cpp
	$(CC) $(CFLAGS) -c updater.cpp

fileIO.o: fileIO.cpp
	$(CC) $(CFLAGS) -c fileIO.cpp

# .o: .cpp
# 	$(CC) $(CFLAGS) -c .cpp

.PHONY: clean
clean:
	rm *.o