#include "server.hpp"

int main(int argc, char* argv[])
{
	auto return_value = 0;
	auto user = unordered_map<int, unordered_set<string>>();
	auto rates = hash_table_t<int>();
	auto nr_rates = hash_table_t<int>();

	auto TCP_socket = socket(AF_INET, SOCK_STREAM, 0);
	DIE(TCP_socket < 0, "TCP Socket creation failed");

	auto port = atoi(argv[1]);
	DIE(!port, "Invalid PORT error");

	auto server_addr = sockaddr_in();
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	socklen_t address_size = sizeof(sockaddr_in);

	return_value = bind(TCP_socket, (sockaddr *)
						&server_addr, address_size);
	DIE(return_value < 0, "TCP bind error");

	return_value = listen(TCP_socket, MAX_SUBSCRIBERS);
	DIE(return_value < 0, "TCP listen error");

	auto read_fds = fd_set();
	FD_SET(STDIN_FILENO, &read_fds);
	FD_SET(TCP_socket, &read_fds);
	auto last_socket = TCP_socket;

	while (true)
	{
		auto tmp_fds = read_fds;

		return_value = select(last_socket + 1, &tmp_fds,
							  nullptr, nullptr, nullptr);
		DIE(return_value < 0, "Select error");

		for (auto socket_id = 0; socket_id <= last_socket; ++socket_id)
		{
			if (FD_ISSET(socket_id, &tmp_fds))
			{
				if (socket_id == STDIN_FILENO)
				{
					auto cmd = string();
					getline(cin, cmd);

					if (cmd == "exit")
					{
						for (auto client_socket = 1; client_socket <= last_socket; ++client_socket)
							if (FD_ISSET(client_socket, &read_fds)
								&& client_socket != TCP_socket)
							{
								return_value = send(client_socket, "exit", 5, 0);
								DIE(return_value < 0, "Sending exit signal failed");
								remove_client(user, read_fds, client_socket);
							}

						close(TCP_socket);
						FD_CLR(TCP_socket, &read_fds);
						shutdown(TCP_socket, SHUT_RDWR);

						return 0;
					}
				}
				else if (socket_id == TCP_socket)
				{
					auto subscriber_addr = sockaddr_in();
					auto new_subscriber = accept(TCP_socket, (sockaddr *)
												 &subscriber_addr, &address_size);
					DIE(new_subscriber < 0, "Accepting new subscriber failed");

					FD_SET(new_subscriber, &read_fds);
					last_socket = max(last_socket, new_subscriber);

					cout << "New client (" << new_subscriber << ") connected from "
						 << ip_port(subscriber_addr) << ".\n";
				}
				else
				{
					auto buffer = array<char, BUFFER_SIZE>();
					return_value = recv(socket_id, buffer.data(), buffer.size(), 0);
					DIE(return_value < 0, "Receiving from subscriber failed");

					if (!return_value)
					{
						remove_client(user, read_fds, socket_id);
					}
					else
					{
						auto message = string(move(buffer.data()));

						if (message.front() == '0') // add
						{
							size_t position = 2u;
							auto game = get_word(message, position);

							rates.insert({game, 0});
							nr_rates.insert({game, 0});
						}
						else if (message.front() == '1') // rate
						{
							size_t position = 2u;
							auto game = get_word(message, position);
							auto rate = stoi(get_word(message, position));

							if (user[socket_id].find(game) == end(user[socket_id])
								&& rates.find(game) != end(rates))
							{
								user[socket_id].insert(game);
								nr_rates[game]++;
								rates[game] += rate;
							}
						}
						else if (message.front() == '2') // list
						{
							for (auto&& [game, rate]: rates)
								if (nr_rates[game])
								{
									auto msg = "Game: " + game + " | Average rating: " +
											to_string(1.0 * rate / nr_rates[game]);
									return_value = send(socket_id, msg.data(), msg.size(), 0);
									DIE(return_value < 0, "Sending from server failed");
									usleep(DELAY);
								}
						}
					}
				}
			}
		}
	}

	return -1;
}
