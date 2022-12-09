server: server.o
	g++ -o server server.o -lcoap-3-gnutls

server.o: server.cpp
	g++ -c -std=c++20 -Wall -pedantic -Werror -o server.o server.cpp

clean:
	rm server server.o


