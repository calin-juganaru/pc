FLAGS = -Wall -std=c++17

ID_client_1 = Gicu
ID_client_2 = Sanda
IP_Server = 127.0.0.1
Port_Server = 8080

build: server client

server: server.cpp
	g++ $(FLAGS) $^ -o server

client: client.cpp
	g++ $(FLAGS) $^ -o client

run_server:
	./server ${Port_Server}

run_client:
	./client

run_client_1:
	./client ${ID_client_1} ${IP_Server} ${Port_Server}

run_client_2:
	./client ${ID_client_2} ${IP_Server} ${Port_Server}

clean:
	rm -f server client
