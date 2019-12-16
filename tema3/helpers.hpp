#pragma once

#include <cstdlib>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <iostream>
using std::cout;

constexpr auto IP_SERVER = "185.118.200.35";
constexpr auto PORT_SERVER  = 8081;
constexpr auto BUFFER_SIZE = 4096u;

void error(const char* message);

int open_connection(const string& host_ip, const int& port, const int& ip_type,
					const int& socket_type, const int& protocol);

void close_connection(const int& sockfd);

void send_to_server(const int& sockfd, const string& message);

string receive_from_server(const int& sockfd);

vector<string> get_cookies(const string& message);

string get_ip(const string& url);

