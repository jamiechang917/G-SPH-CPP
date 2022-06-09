OBJ = main.o myvector.o particle.o tree.o force.o updater.o fileIO.o gpuplugin.o
CC = nvcc
# CFLAGS = -Wall -Wextra -O2 -g
CFLAGS = -O2 -g -arch=sm_60 # Require Compute Capability 6.0+
LFLAGS = -lm

program: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o program  $(LFLAGS)

main.o: main.cu
	$(CC) $(CFLAGS) -c main.cu

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

gpuplugin.o: gpuplugin.cu
	$(CC) $(CFLAGS) -c gpuplugin.cu

# .o: .cpp
# 	$(CC) $(CFLAGS) -c .cpp

.PHONY: clean
clean:
	rm *.o