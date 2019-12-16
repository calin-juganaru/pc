FLAGS = -std=c++11 -O3 -Wall

build: client.cpp requests.cpp helpers.cpp json.hpp tasks.hpp
		g++ $(FLAGS) client.cpp requests.cpp helpers.cpp -o client

run: client
		./client

clean:
		rm -f *.o client
