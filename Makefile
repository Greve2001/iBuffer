CC=gcc
CFLAGS=-Wall -s -O3

objects=discover.o linklist.o main.o tcpclient.o tcpserver.o tui.o parser.o

iBuffer: $(objects) -lpthread -lncurses
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c common.h linklist.h -lpthread
	$(CC) -c $< $(CLAGS) -o $@

tui.o: tui.c common.h linklist.h -lncurses
	$(CC) -c $< $(CLAGS) -o $@

linklist.o: linklist.c common.h 
	$(CC) -c $< $(CFLAGS) -o $@

discover.o: discover.c common.h -lpthread
	$(CC) -c $< $(CFLAGS) -o $@

parser.o: parser.c common.h
	$(CC) -c $< $(CFLAGS) -o $@

tcpclient.o: tcpclient.c common.h -lpthread
	$(CC) -c $< $(CFLAGS) -o $@

tcpserver.o: tcpserver.c common.h -lpthread
	$(CC) -c $< $(CFLAGS) -o $@

.PHONY: clean
clean:
	-rm -rf *.o *.gch iBuffer
