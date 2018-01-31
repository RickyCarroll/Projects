CC = gcc
CFLAGS = -g -Wall

ush: ush.o expand.o builtin.o
	$(CC) $(CFLAGS) -o ush ush.o expand.o builtin.o
ush.o: ush.c
	$(CC) $(CFLAGS) -c ush.c defn.h
expand.o: expand.c
	$(CC) $(CFLAGS) -c expand.c defn.h
builtin.o: builtin.c
	$(CC) $(CFLAGS) -c builtin.c defn.h

clean:
	$(RM) ush *.o *~ *.gch
