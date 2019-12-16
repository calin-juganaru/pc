#include "utils.hpp"

int main(int argc, char* argv[])
{
	auto server_addr = sockaddr_in();
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[3]));

	auto TCP_socket = socket(AF_INET, SOCK_STREAM, 0);
	DIE(TCP_socket < 0, "TCP Socket creation failed");

	auto return_value = inet_aton(argv[2], &server_addr.sin_addr);
	DIE(return_value == 0, "inet_aton error");

	return_value = connect(TCP_socket, (sockaddr*) &server_addr,
						   sizeof(server_addr));
	DIE(return_value < 0, "Could not connect to server");

	auto read_fds = fd_set();
	FD_SET(STDIN_FILENO, &read_fds);
	FD_SET(TCP_socket, &read_fds);

	auto id = string(argv[1]);
	return_value = send(TCP_socket, id.data(), id.size(), 0);
	DIE(return_value < 0, "Send addr ID to server error");

	while (true)
	{
		auto tmp_fds = read_fds;
		return_value = select(TCP_socket + 1, &tmp_fds,
							  nullptr, nullptr, nullptr);
		DIE(return_value < 0, "Select failure");

		if (FD_ISSET(STDIN_FILENO, &tmp_fds))
		{
			auto cmd = string();
			getline(cin, cmd);
			cmd += " ";

			if (cmd == "exit") break;
			else if (cmd.substr(0, 3) == "add")
			{
				size_t position = 4u;
				auto game = get_word(cmd, position);
				auto message = "0 " + game + " ";

				return_value = send(TCP_socket, message.data(), message.size(), 0);
				DIE(return_value < 0, "Send to server add cmd failed");

				cout << "added " << game << "\n";
			}
			else if (cmd.substr(0, 4) == "rate")
			{
				size_t position = 5;
				auto game = get_word(cmd, position);
				auto rate = get_word(cmd, position);
				auto message = "1 " + game + " " + rate + " ";

				return_value = send(TCP_socket, message.data(), message.size(), 0);
				DIE(return_value < 0, "Send to server rate cmd failed");

				cout << "rated " << game << " " << rate << "\n";
			}
			else if (cmd.substr(0, 4) == "list")
			{
				auto message = string("2 ");

				return_value = send(TCP_socket, message.data(), message.size(), 0);
				DIE(return_value < 0, "Send to server list cmd failed");
			}
			else cout << "Invalid command.\n";
		}
		else
		{
			if (FD_ISSET(TCP_socket, &tmp_fds))
			{
				auto buffer = array<char, BUFFER_SIZE>();
				return_value = recv(TCP_socket, begin(buffer), buffer.size(), 0);
				DIE(return_value < 0, "Receive from server error");

				if (equal(begin(buffer), begin(buffer) + 5, "exit")) break;

				cout << buffer.data() << endl;
			}
		}
	}

	close(TCP_socket);
	FD_CLR(TCP_socket, &read_fds);
}
