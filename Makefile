OBJ = main.o particles.o fileIO.o randoms.o BHtree.o vector_ops.o
CC = g++
CFLAGS = -Wall -O2 -g
LFLAGS = -lm
# CFLAGS = -Wall -O2 -I/usr/include/python3.8 -g
# LFLAGS = -lm -lpython3.8

program: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o program  $(LFLAGS)

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp

particles.o: particles.cpp particles.h
	$(CC) $(CFLAGS) -c particles.cpp

fileIO.o: fileIO.cpp fileIO.h
	$(CC) $(CFLAGS) -c fileIO.cpp

randoms.o: randoms.cpp randoms.h
	$(CC) $(CFLAGS) -c randoms.cpp

BHtree.o: BHtree.cpp BHtree.h
	$(CC) $(CFLAGS) -c BHtree.cpp

vector_ops.o: vector_ops.cpp vector_ops.h
	$(CC) $(CFLAGS) -c vector_ops.cpp

.PHONY: clean
clean:
	rm *.o