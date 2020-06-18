all:client.c server.c
	gcc -g -Wall -o client client.c -lpthread
	gcc -g -Wall -o server server.c -lpthread

clean:
	rm -rf *.o client server 
