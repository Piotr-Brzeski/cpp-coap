client: client.o
	g++ -o client client.o -lcoap-3-gnutls

client.o: client.cpp
	g++ -c -std=c++20 -Wall -pedantic -Werror -o client.o client.cpp

clean:
	rm client client.o


