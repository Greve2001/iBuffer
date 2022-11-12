CC=gcc
CFLAGS=-g 

objects=discover.o linklist.o main.o tcpclient.o tcpserver.o tui.o

iBuffer: $(objects)
	$(CC) $(CFLAGS) -lncurses -lpthread $(objects) -o $@

tui.o: tui.c common.h
	$(CC) -c tui.c $(CLAGS) -lncurses -o $@

linklist.o: linklist.c common.h linklist.h
	$(CC) -c linklist.c $(CFLAGS) -o $@

discover.o: discover.c common.h
	$(CC) -c discover.c $(CFLAGS) -lpthread -o $@

clean:
	rm -rf *.o iBuffer
