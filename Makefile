all: server.c client.c
	gcc -c bank.c
	gcc -c clientHandle.c
	gcc -w -o server server.c -lpthread clientHandle.o bank.o
	gcc -w -o client client.c -lpthread

clean:
	rm *.o
	rm ./server
	rm ./client
