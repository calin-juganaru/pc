#include "helpers.hpp"

// afișează un mesaj de eroare și iese din program
void error(const char* message)
{
    perror(message);
    exit(0);
}

// deschide o nouă conexiune și întoarce descriptorul pentru noul socket
int open_connection(const string& host_ip, const int& port, const int& ip_type,
                    const int& socket_type, const int& protocol)
{
    auto serv_addr = sockaddr_in();
    auto sockfd = socket(ip_type, socket_type, protocol);

    serv_addr.sin_family = ip_type;
    serv_addr.sin_port = htons(port);
    inet_aton(host_ip.data(), &serv_addr.sin_addr);

    if (sockfd < 0)
        error("ERROR opening socket");

    if (connect(sockfd, (sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;
}

// închide conexiunea
void close_connection(const int& sockfd)
{
    close(sockfd);
}

// trimite date la server
void send_to_server(const int& sockfd, const string& message)
{
    auto sent = size_t(), total = message.size();

    do
    {
        auto bytes = write(sockfd, message.data() + sent, total - sent);

        if (bytes < 0)
            error("ERROR writing message to socket");
        if (bytes == 0)
            break;

        sent += bytes;
    }
    while (sent < total);
}

// primește date de la server
string receive_from_server(const int& sockfd)
{
    auto response = vector<char>(BUFFER_SIZE);
    auto total = BUFFER_SIZE, received = 0u;

    do
    {
        auto bytes = read(sockfd, &response[received], total - received);

        if (bytes < 0)
            error("ERROR reading response from socket");

        if (bytes == 0)
            break;

        received += bytes;
    }
    while (received < total);

    if (received == total)
        error("ERROR storing complete response from socket");

    return response.data();
}

// obțin cookie-urile dintr-un mesaj
vector<string> get_cookies(const string& message)
{
    static constexpr auto magic_string = "Set-Cookie: ";
    static constexpr auto magic_string_size = 12;

    auto cookies = vector<string>();
    auto start = message.find(magic_string);

    while (start != string::npos)
    {
        start += magic_string_size;
        auto stop = message.find(';', start);
        cookies.push_back(message.substr(start, stop - start));
        start = message.find(magic_string, stop);
    }

    return cookies;
}

// obțin un ip corespunzător unui url
string get_ip(const string& url)
{
	auto ip = string(16, 0);
	addrinfo* result = nullptr;

	auto hints = addrinfo();
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = nullptr;
	hints.ai_addr = nullptr;
	hints.ai_next = nullptr;

	auto ret = getaddrinfo(url.data(), nullptr, &hints, &result);
    if (ret)
	{
        printf("getaddrinfo: %s\n", gai_strerror(ret));
        exit(1);
    }

    auto saddr = (sockaddr_in*)result->ai_addr;
    inet_ntop(result->ai_family, &(saddr->sin_addr), &ip[0], sizeof(ip));
    freeaddrinfo(result);

    while (!ip.back())
        ip.pop_back();

    return ip;
}