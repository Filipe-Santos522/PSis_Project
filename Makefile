# C compiler
HOST_CC = gcc  -O2

HOST_CC_FLAGS =  -lncurses

server_host: server.o
	$(HOST_CC) server.o  -o server_host $(HOST_CC_FLAGS)

client: client.o
	$(HOST_CC) client.o  -o client $(HOST_CC_FLAGS)



server.o: server.c Serverfunc.h single-pong.h sock_dg_inet.h pong.h
	$(HOST_CC) -c server.c -o server.o  $(HOST_CC_FLAGS)

client.o: client.c Serverfunc.h single-pong.h sock_dg_inet.h pong.h
	$(HOST_CC) -c client.c -o client.o  $(HOST_CC_FLAGS)


clean:
	rm -f *.o *~ server_host client
