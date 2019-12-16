#include "helpers.hpp"
#include "requests.hpp"
#include "json.hpp"
#include "tasks.hpp"

int main(int argc, char* argv[])
{
    // ------------------------------------ Task 1 ------------------------------------
    auto sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    auto cookies = vector<string>();
    auto message = compute_get_request(IP_SERVER, "/task1/start", "", "", cookies);
    send_to_server(sockfd, message);
    message = receive_from_server(sockfd);
    auto task1 = parse_json(message).get<Task1>();
    close_connection(sockfd);
    // --------------------------------------------------------------------------------

    // ------------------------------------ Task 2 ------------------------------------
    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request(IP_SERVER, task1.url, task1.type,
                                   "", cookies, task1.form_data());
    send_to_server(sockfd, message);
    message = receive_from_server(sockfd);
    auto task2 = parse_json(message).get<Task2>();
    cookies = get_cookies(message);
    close_connection(sockfd);
    // --------------------------------------------------------------------------------

    // ------------------------------------ Task 3 ------------------------------------
    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(IP_SERVER, task2.url, task2.get_answer(),
                                  task2.data.token, cookies);
    send_to_server(sockfd, message);
    message = receive_from_server(sockfd);
    auto task3 = parse_json(message).get<Task3>();
    cookies = get_cookies(message);
    close_connection(sockfd);
    // --------------------------------------------------------------------------------

    // ------------------------------------ Task 4 ------------------------------------
    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(IP_SERVER, task3.url, "", task2.data.token, cookies);
    send_to_server(sockfd, message);
    message = receive_from_server(sockfd);
    auto task4 = parse_json(message).get<Task4>();
    cookies = get_cookies(message);
    close_connection(sockfd);
    // --------------------------------------------------------------------------------

    // ----------------------------------- Task 5.1 -----------------------------------
    sockfd = open_connection(get_ip(task4.get_url()), 80, AF_INET, SOCK_STREAM, 0);
    message = compute_get_request(get_ip(task4.get_url()), task4.data.get_url(),
                                  task4.get_params(), "", vector<string>());
    send_to_server(sockfd, message);
    message = receive_from_server(sockfd);
    close_connection(sockfd);
    // --------------------------------------------------------------------------------

    // ----------------------------------- Task 5.2 -----------------------------------
    sockfd = open_connection(IP_SERVER, PORT_SERVER, AF_INET, SOCK_STREAM, 0);
    message = compute_post_request(IP_SERVER, task4.url, task4.type, task2.data.token,
                                   cookies, parse_json(message).dump());
    send_to_server(sockfd, message);
    message = receive_from_server(sockfd);
    cout << newline << message << newline;
    close_connection(sockfd);
    // --------------------------------------------------------------------------------
}