#pragma once

#include "utils.hpp"

template <typename T>
using hash_table_t = unordered_map<string, T>;

static constexpr auto DELAY = 100;
static constexpr auto MAX_SUBSCRIBERS = 20;

// funcție pentru închiderea conexiunii cu un client TCP
void remove_client(unordered_map<int, unordered_set<string>>& user,
				   fd_set& read_fds, const int& socket)
{
	cout << "Client (" << socket << ") disconnected.\n";
	close(socket);
	FD_CLR(socket, &read_fds);
	user.erase(socket);
}

// obține un string port:ip de la o adresă
string ip_port(const sockaddr_in& address)
{
	return string(inet_ntoa(address.sin_addr)) + ":"
		   + to_string(ntohs(address.sin_port));
}