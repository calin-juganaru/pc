#include "utils.hpp"

int main(int argc, char* argv[])
{
	// inițializez structura pentru adresa server-ului
	auto server_addr = sockaddr_in();
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[3]));

	// deschid socket-ul TCP
	auto TCP_socket = socket(AF_INET, SOCK_STREAM, 0);
	DIE(TCP_socket < 0, "TCP Socket creation failed");

	// folosind IP-ul server-ului primit ca parametru
	auto return_value = inet_aton(argv[2], &server_addr.sin_addr);
	DIE(return_value == 0, "inet_aton error");

	// mă conectez la server
	return_value = connect(TCP_socket, (sockaddr*) &server_addr,
						   sizeof(server_addr));
	DIE(return_value < 0, "Could not connect to server");

	// inițializez lista de descriptori
	auto read_fds = fd_set();
	FD_SET(STDIN_FILENO, &read_fds);
	FD_SET(TCP_socket, &read_fds);

	// trimit către server ID-ul clientului
	auto id = string(argv[1]);
	return_value = send(TCP_socket, id.data(), id.size(), 0);
	DIE(return_value < 0, "Send Subscriber ID to server error");

	while (true)
	{
		auto tmp_fds = read_fds;
		return_value = select(TCP_socket + 1, &tmp_fds,
							  nullptr, nullptr, nullptr);
		DIE(return_value < 0, "Select failure");

		if (FD_ISSET(STDIN_FILENO, &tmp_fds))
		{
			// primesc comanda de la stdin
			auto cmd = string();
			getline(cin, cmd);

			if (cmd == "exit") break;
			else if (cmd.substr(0, 9) == "subscribe")
			{
				// parsez comanda
				size_t position = 10u;
				auto topic = get_word(cmd, position);
				auto message = topic + " " + id + " "
							 + cmd.substr(position, 1) + "1";

				// trimit comanda de subscribe la server
				return_value = send(TCP_socket, message.data(), message.size(), 0);
				DIE(return_value < 0, "Send to server subscribe cmd failed");

				cout << "subscribed " << topic << "\n";
			}
			else if (cmd.substr(0, 11) == "unsubscribe")
			{
				// parsez comanda, dar adaug un spațiu la început
				//	pentru funcția de parsare get_word()
				cmd += " ";
				size_t position = 12;
				auto topic = get_word(cmd, position);
				auto message = topic + " " + id + " 0";

				// trimit comanda de unsubscribe la server
				return_value = send(TCP_socket, message.data(), message.size(), 0);
				DIE(return_value < 0, "Send to server unsubscribe cmd failed");

				cout << "unsubscribed " << topic << "\n";
			}
			// orice altă comandă nu este permisă
			else cout << "Invalid subscriber command.\n";
		}
		else
		{
			// dacă primesc un mesaj de la server
			if (FD_ISSET(TCP_socket, &tmp_fds))
			{
				auto buffer = array<char, BUFFER_SIZE>();
				return_value = recv(TCP_socket, begin(buffer), buffer.size(), 0);
				DIE(return_value < 0, "Receive from server error");

				// dacă este de exit, clientul se va opri
				if (equal(begin(buffer), begin(buffer) + 5, "exit")) break;

				// altfel, este afișat
				cout << buffer.data() << endl;
			}
		}
	}

	close(TCP_socket);
	FD_CLR(TCP_socket, &read_fds);
}
