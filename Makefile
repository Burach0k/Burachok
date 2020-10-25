CC = gcc
32 = i686-w64-mingw32-gcc -lmingw32
64 = x84_64-w64-mingw32-gcc
CFLAGS = -L/usr/lib/ssl

all:
	$(CC) $(CFLAGS) main.c -lssl -lcrypto

clean:
	rm -rf *.o
	rm -rf *.out