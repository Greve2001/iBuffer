CC=gcc
CFLAGS=-g -lncurses

objects=discover.o linklist.o main.o tcpclient.o tcpserver.o tui.o

iBuffer: $(objects)
	$(CC) $(CFLAGS) $(objects) -o $@

tui.o: tui.c common.h
	$(CC) -c tui.c $(CLAGS) -lncurses

linklist.o: linklist.c common.h linklist.h
	$(CC) -c linklist.c $(CFLAGS) -o $@

clean:
	rm -rf *.o iBuffer
