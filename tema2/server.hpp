#pragma once

#include "utils.hpp"

template <typename T>
using hast_table_t = unordered_map<string, T>;

static constexpr auto DELAY = 100;
static constexpr auto MAX_SUBSCRIBERS = 20;

static const string data_types[] = {"INT", "SHORT_REAL", "FLOAT", "STRING"};

// funcție pentru închiderea conexiunii cu un client TCP
void remove_client(hast_table_t<hast_table_t<pair<bool, bool>>>& subscribers,
				   hast_table_t<unordered_set<string>>& topics,
				   unordered_map<int, string>& socket_to_id,
				   hast_table_t<int>& id_to_socket,
				   fd_set& read_fds, const int& socket)
{
	auto id = socket_to_id[socket];
	if (!id.empty())
		cout << "Client (" << id << ") disconnected.\n";

	// închid conexiunea
	close(socket);
	FD_CLR(socket, &read_fds);

	// pentru fiecare topic la care este abonat clientul
	for (auto&& topic: topics[id])
	{
		// îl dezabonez
		subscribers[topic][id].first = false;
	}
	// și-l șterg din structuri
	topics[id].clear();
	id_to_socket.erase(id);
	socket_to_id.erase(socket);
}

// obține un număr întreg din mesajul de la un client UDP
string make_int(const char* buffer)
{
	auto aux = string();

	for (auto&& i: {0, 1, 2, 3})
		aux += bitset<8>(buffer[i]).to_string();

	return to_string(bitset<32>(aux).to_ulong());
}

// obține un număr short_real din mesajul de la un client UDP
string make_real(const char* buffer)
{
	auto aux = string();

	for (auto&& i: {0, 1})
		aux += bitset<8>(buffer[i]).to_string();

	return to_string(bitset<16>(aux).to_ulong());
}

// obține un număr float din mesajul de la un client UDP
string make_float(const char* buffer)
{
	auto aux = string();

	for (auto&& i: {0, 1, 2, 3})
		aux += bitset<8>(buffer[i]).to_string();

	auto number = to_string(bitset<32>(aux).to_ulong());
	auto exp = bitset<8>(buffer[4]).to_ulong();

	if (exp)
	{
		while (number.size() <= exp)
			number.insert(0, "0");
		number.insert(number.size() - exp, ".");
	}

	return number;
}

// obține un șir de caractere din mesajul de la un client UDP
string parse_message(const char* buffer, const size_t& data_type)
{
	auto message = string();

	switch (data_type)
	{
		case 0:
		{
			message += bitset<8>(buffer[0]).to_ulong() ? "-" : "";
			message += make_int(buffer + 1);
			break;
		}

		case 1:
		{
			message += make_real(buffer + 0);
			break;
		}

		case 2:
		{
			message += bitset<8>(buffer[0]).to_ulong() ? "-" : "";
			message += make_float(buffer + 1);
			break;
		}

		case 3:
		{
			message += buffer + 0;
			break;
		}

		default: break;
	}

	return message;
}

// obține un string port:ip de la o adresă
string ip_port(const sockaddr_in& address)
{
	return string(inet_ntoa(address.sin_addr)) + ":"
		   + to_string(ntohs(address.sin_port));
}
