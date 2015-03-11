all: cheeky-server
cheeky-server: cheeky-server.c
	gcc -o cheeky-server cheeky-server.c
clean:	
	rm cheeky-server