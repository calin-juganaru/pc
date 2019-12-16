#include "server.hpp"

int main(int argc, char* argv[])
{
	auto return_value = 0, TRUE = 1;
	// subscribers[topic][id] = {subscribe, store_forward};
	auto subscribers = hast_table_t<hast_table_t<pair<bool, bool>>>();
	// id_to_socket[id] = socket;
	auto id_to_socket = hast_table_t<int>();
	// socket_to_id[socket] = id;
	auto socket_to_id = unordered_map<int, string>();
	// store_buffer[id][topic] = messages[];
	auto store_buffer = hast_table_t<hast_table_t<vector<string>>>();
	// topics[id] = topics[];
	auto topics = hast_table_t<unordered_set<string>>();

	// deschid socket-ul TCP
	auto TCP_socket = socket(AF_INET, SOCK_STREAM, 0);
	DIE(TCP_socket < 0, "TCP Socket creation failed");

	// deschid socket-ul UDP
	auto UDP_socket = socket(AF_INET, SOCK_DGRAM, 0);
    DIE(UDP_socket < 0, "UDP Socket creation failed");

	// obțin port-ul din parametru
	auto port = atoi(argv[1]);
	DIE(!port, "Invalid PORT error");

	// inițializez structura pentru adresa server-ului
	auto server_addr = sockaddr_in();
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;
	socklen_t address_size = sizeof(sockaddr_in);

	// pornesc conexiunea TCP
	return_value = bind(TCP_socket, (sockaddr *)
						&server_addr, address_size);
	DIE(return_value < 0, "TCP bind error");

	return_value = listen(TCP_socket, MAX_SUBSCRIBERS);
	DIE(return_value < 0, "TCP listen error");

	// pornesc conexiunea UDP
	DIE(setsockopt(UDP_socket, SOL_SOCKET, SO_REUSEADDR, &TRUE,
		sizeof(int)) < 0, "setsockopt(SO_REUSEADDR) failed");

    return_value = bind(UDP_socket, (sockaddr *)
						&server_addr, address_size);
    DIE(return_value < 0, "Socket Binding Failed");

	// inițializez lista de descriptori
	auto read_fds = fd_set();
	FD_SET(STDIN_FILENO, &read_fds);
	FD_SET(UDP_socket, &read_fds);
	FD_SET(TCP_socket, &read_fds);
	auto last_socket = TCP_socket;

	while (true)
	{
		auto tmp_fds = read_fds;

		return_value = select(last_socket + 1, &tmp_fds,
							  nullptr, nullptr, nullptr);
		DIE(return_value < 0, "Select error");

		// iterez prin mulțimea de socket-uri active
		for (auto socket_id = 0; socket_id <= last_socket; ++socket_id)
		{
			if (FD_ISSET(socket_id, &tmp_fds))
			{
				// Cazul 1: stdin
				if (socket_id == STDIN_FILENO)
				{
					// primesc comanda
					auto cmd = string();
					getline(cin, cmd);

					if (cmd == "exit")
					{
						// închid toate conexiunile și ies din program cu succes
						for (auto client_socket = 1; client_socket <= last_socket; ++client_socket)
							if (FD_ISSET(client_socket, &read_fds)
								&& client_socket != TCP_socket
								&& client_socket != UDP_socket)
							{
								return_value = send(client_socket, "exit", 5, 0);
								DIE(return_value < 0, "Sending exit signal failed");
								remove_client(subscribers, topics, socket_to_id,
									  		  id_to_socket, read_fds, client_socket);
							}

						// închid la final și conexiunea cu TCP
						close(TCP_socket);
						FD_CLR(TCP_socket, &read_fds);
						shutdown(TCP_socket, SHUT_RDWR);
						// și returnez valoarea de succes
						return 0;
					}
				}
				// Cazul 2: un nou client TCP
				else if (socket_id == TCP_socket)
				{
					// accept o nouă conexiune cu un abonat
					auto subscriber_addr = sockaddr_in();
					auto new_subscriber = accept(TCP_socket, (sockaddr *)
												 &subscriber_addr, &address_size);
					DIE(new_subscriber < 0, "Accepting new subscriber failed");

					// îl adaug în mulțimea de sockeți
					FD_SET(new_subscriber, &read_fds);
					last_socket = max(last_socket, new_subscriber);

					// primesc de la client ID-ul
					auto id = array<char, ID_SIZE>();
					return_value = recv(new_subscriber, id.data(), id.size(), 0);
					DIE(return_value < 0, "Receiving ID from TCP Client failed");

					// dacă ID-ul nu există deja, adaug noul abonat
					if (id_to_socket.find(id.data()) == end(id_to_socket))
					{
						id_to_socket[id.data()] = new_subscriber;
						socket_to_id[new_subscriber] = id.data();

						cout << "New client (" << id.data() << ") connected from "
						 	 << ip_port(subscriber_addr) << ".\n";
					}
					// altfel, resping cererea de conexiune
					else
					{
						return_value = send(new_subscriber, "exit", 5, 0);
						DIE(return_value < 0, "Sending exit signal failed");
						remove_client(subscribers, topics, socket_to_id,
									  id_to_socket, read_fds, new_subscriber);
					}

				}
				// Cazul 3: publisher (UDP)
				else if (socket_id == UDP_socket)
				{
					auto publisher_addr = sockaddr_in();
					auto buffer = array<char, BUFFER_SIZE>();

					// primesc mesajul de la publisher
					return_value = recv(socket_id, buffer.data(), buffer.size(), 0);
					DIE(return_value < 0, "Receiving from UDP Client failed");

					// parsez mesajul pentru a-l trimite la abonați
					auto topic = string(buffer.data());
					auto data_type = bitset<8>(buffer[50]).to_ulong();
					auto message = ip_port(publisher_addr) + " - " + topic
								 + " - " + data_types[data_type] + " - "
								 + parse_message(buffer.data() + 51, data_type);

					// pentru fiecare abonat la acest topic
					for (auto&& it: subscribers[topic])
					{
						// dacă este abonat (activ)
						if (it.second.first)
						{
							// îi trimit mesajul
							return_value = send(id_to_socket[it.first], message.data(),
												message.size(), 0);
							DIE(return_value < 0, "Sending subscription failed");
							usleep(DELAY);
						}
						// dacă nu mai este abonat, dar SF-ul este setat
						else if (!it.second.first && it.second.second)
						{
							// stochez mesajul
							store_buffer[it.first][topic].push_back(message);
						}
					}
				}
				// Cazul 4: abonații
				else
				{
					// primesc comanda de la clientul TCP
					auto buffer = array<char, BUFFER_SIZE>();
					return_value = recv(socket_id, buffer.data(), buffer.size(), 0);
					DIE(return_value < 0, "Receiving from subscriber failed");

					// când abonatul dorește să se deconecteze
					if (!return_value)
					{
						remove_client(subscribers, topics, socket_to_id,
									  id_to_socket, read_fds, socket_id);
					}
					// sau când un subscriber se (dez)abonează (de) la un topic
					else
					{
						// parsez comanda primită
						auto message = string(move(buffer.data()));
						size_t position = 0u;
						auto topic = get_word(message, position);
						auto id = get_word(message, position);

						if (message.back() == '1') // subscribe
						{
							// obțin flag-ul SF
							message.pop_back();
							auto store_forward = (message.back() == '1');

							// actualizez abonarea
							subscribers[topic][id] = {true, store_forward};
							topics[id].insert(topic);

							// și îi trimit mesajele SF din absența sa
							for (auto&& stored_message: store_buffer[id][topic])
							{
								return_value = send(id_to_socket[id], stored_message.data(),
										   			stored_message.size(), 0);
								DIE(return_value < 0, "Sending subscription failed");
								usleep(DELAY);
							}
							// apoi le șterg din buffer
							store_buffer[id][topic].clear();
						}
						else // unubscribe
						{
							// elimin legăturile cu abonatul din structuri
							topics[id].erase(topic);
							subscribers[topic][id].first = false;
						}
					}
				}
			}
		}
	}
	// dacă algoritmul nu a funcționat corect, trimit codul de eroare
	return -1;
}
