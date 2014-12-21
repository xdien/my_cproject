CC=g++
# Hey!, I am comment number 2. I want to say that CFLAGS will be the
# options I'll pass to the compiler. them option "-c" de ra file ".o"
INCLUDES = -I/usr/include/boost
CFLAGS=-c -Wall ${INCLUDES} -Wwrite-strings -pipe -fPIC -march=native

all: udpserver udpclient test_

udpserver: myUDPserver.o
	$(CC) -lboost_system myUDPserver.o  -o udpserver
	strip udpserver
udpclient: test_asio.o
	$(CC) -lboost_system test_asio.o -o udpclient
	strip udpclient
test_: test_.o
	$(CC) -lboost_system test_.o -o test_
	strip udpclient
myUDPserver.o: myUDPserver.cpp
	$(CC) $(CFLAGS) myUDPserver.cpp

test_asio.o: test_asio.cpp
	$(CC) $(CFLAGS) test_asio.cpp
test_.o: test_.cpp
	$(CC) $(CFLAGS) test_.cpp

clean:
	rm -rf *o udpserver  udpclient