all:client.c server.c
	gcc -g -Wall -o client client.c
	gcc -g -Wall -o server server.c

clean:
	rm -rf *.o client server Unix.domain