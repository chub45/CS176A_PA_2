output: PingClient.o
	gcc PingClient.o -o PingClient
	rm *.o

PingClient.o: PingClient.c
	gcc -c PingClient.c

clean:
	rm *.out


