FLAGS = -Wall -std=c++11

ID_Subscriber_1 = Gicu
ID_Subscriber_2 = Sanda
IP_Server = 127.0.0.1
Port_Server = 8080

build: server subscriber

server: server.cpp
	g++ $(FLAGS) $^ -o server

subscriber: subscriber.cpp
	g++ $(FLAGS) $^ -o subscriber

run_server:
	./server ${Port_Server}

run_subscriber:
	./subscriber

run_subscriber_1:
	./subscriber ${ID_Subscriber_1} ${IP_Server} ${Port_Server}

run_subscriber_2:
	./subscriber ${ID_Subscriber_2} ${IP_Server} ${Port_Server}

clean:
	rm -f server subscriber
