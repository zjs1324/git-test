all:client.c server.c
	gcc -g -Wall -o client client.c
	gcc -g -Wall -0 server server.c

clean:
	rm -rf *.o client server