README

To Build use the following commands:


Creation of the server executable:

g++ `std=c++11` serverFinalfinal.cpp person.cpp person.h -o server

Creation of the client executable:

g++ `std=c++11` clientside.cpp person.cpp person.h -o client

To run the programs do the following commands (note run the server first):

./server <port number>

ex)  ./server 12000


./client <address> <port number of server>


ex) ./client eros.cs.txstate.edu 12000


NOTE: The project was turned into as C++ rather than C, I had permission to do so
