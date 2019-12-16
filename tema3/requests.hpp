#pragma once

#include <vector>
using std::vector;

#include <string>
using std::string;
using std::to_string;

string compute_get_request(const string& host, const string& url,
						   const string& url_params, const string& token,
						   const vector<string>& cookies);

string compute_post_request(const string& host, const string& url,
							const string& type, const string& token,
                            const vector<string>& cookies,
							const string& form_data);

constexpr auto newline = "\r\n";