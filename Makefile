CC=gcc
CFLAGS=-g -lncurses -lpthread

objects=discover.o linklist.o main.o tcpclient.o tcpserver.o tui.o parser.o

iBuffer: $(objects)
	$(CC) $(CFLAGS) $(objects) -o $@

main.o: main.c common.h linklist.h
	$(CC) -c main.c $(CLAGS) -o $@

tui.o: tui.c common.h
	$(CC) -c tui.c $(CLAGS) -o $@

linklist.o: linklist.c common.h linklist.h
	$(CC) -c linklist.c $(CFLAGS) -o $@

discover.o: discover.c common.h
	$(CC) -c discover.c $(CFLAGS) -o $@

parser.o: parser.c common.h
	$(CC) -c parser.c $(CFLAGS) -o $@

tcpclient.o: tcpclient.c common.h
	$(CC) -c tcpclient.c $(CFLAGS) -o $@

tcpserver.o: tcpclient.c common.h
	$(CC) -c tcpserver.c $(CFLAGS) -o $@

clean:
	rm -rf *.o *.gch iBuffer
