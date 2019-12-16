#include "requests.hpp"

string compute_get_request(const string& host, const string& url,
                           const string& url_params, const string& token,
                           const vector<string>& cookies)
{
    auto message = "GET " + url;

    if (!url_params.empty())
        message += "?" + url_params;

    message += " HTTP/1.1\r\n";
    message += "Host: " + host + newline;

    if (!token.empty())
        message += "Authorization: Basic " + token + newline;

    if (!cookies.empty())
    {
        message += "Cookie:";
        for (auto&& cookie: cookies)
            message += " " + cookie + ";";
        message += newline;
    }

    return message + newline;
}

string compute_post_request(const string& host, const string& url,
                            const string& type, const string& token,
                            const vector<string>& cookies,
                            const string& form_data)
{
    auto message = "POST " + url + " HTTP/1.1" + newline;

    message += "Host: " + host + newline;

    if (!token.empty())
        message += "Authorization: Basic " + token + newline;

    if (!cookies.empty())
    {
        message += "Cookie:";
        for (auto&& cookie: cookies)
            message += " " + cookie + ";";
        message += newline;
    }

    message += "Content-Type: " + type + newline;
    message += "Content-Length: " + to_string(form_data.size()) + newline;
    message += newline + form_data + newline + newline;

    return message;
}