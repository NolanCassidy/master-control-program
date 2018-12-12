

TARGET = P1 P2 P3 P4 P5 
LIBS = -lm
CC = gcc
CFLAGS = -g -Wall


all: P1 P2 P3 P4 P5 

P1.o: P1.c
	$(CC) $(CFLAGS) -c $< -o $@

P2.o: P2.c
	$(CC) $(CFLAGS) -c $< -o $@

P3.o: P3.c
	$(CC) $(CFLAGS) -c $< -o $@

P4.o: P4.c
	$(CC) $(CFLAGS) -c $< -o $@

P5.o: P5.c
	$(CC) $(CFLAGS) -c $< -o $@


P1: P1.o 
	$(CC) -Wall $(LIBS) $^ -o $@

P2: P2.o 
	$(CC) -Wall $(LIBS) $^ -o $@

P3: P3.o 
	$(CC) -Wall $(LIBS) $^ -o $@

P4: P4.o 
	$(CC) -Wall $(LIBS) $^ -o $@

P5: P5.o 
	$(CC) -Wall $(LIBS) $^ -o $@


clean:
	rm -f *.o
	rm -f P1 P2 P3 P4 P5
