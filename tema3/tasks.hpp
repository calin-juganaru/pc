#pragma once

#include "json.hpp"
using nlohmann::json;

#include <string>
using std::string;

struct Task
{
	string enunt, url, method;
};

struct Task1: Task
{
	string type;

	struct
	{
		string username, password;
	} data;

	string form_data()
	{
		return "user=" + data.username +
			   "&pass=" + data.password;
	}
};

struct Task2: Task
{
	struct
	{
		string token;

		struct
		{
			string id;
		} queryParams;
	} data;

	string get_answer()
	{
		return "raspuns1=omul&raspuns2=numele&id="
				+ data.queryParams.id;
	}
};

struct Task3: Task{};

struct Task4: Task
{
	string type;

	struct
	{
		string method, url;

		struct
		{
			string q, APPID;
		} queryParams;

		string get_url()
		{
			auto start = url.find('/');
			return url.substr(start, url.size() - start);
		}
	} data;

	string get_params()
	{
		return "q=" + data.queryParams.q + "&appid="
				+ data.queryParams.APPID;
	}

	string get_url()
	{
		auto stop = data.url.find('/');
		return data.url.substr(0, stop);
	}
};

inline void to_json(json& jason, const Task1& task)
{
	jason = json
	{
		{"enunt", task.enunt},
		{"url", task.url},
		{"method", task.method},
		{"type", task.type},
		{
			"data",
				{"username", task.data.username},
				{"password", task.data.password}
		}
	};
}

inline void from_json(const json& jason, Task1& task)
{
	jason["enunt"].get_to(task.enunt);
	jason["url"].get_to(task.url);
	jason["method"].get_to(task.method);
	jason["type"].get_to(task.type);
	jason["data"]["username"].get_to(task.data.username);
	jason["data"]["password"].get_to(task.data.password);
}

inline void to_json(json& jason, const Task2& task)
{
	jason = json
	{
		{"enunt", task.enunt},
		{"url", task.url},
		{"method", task.method},
		{
			"data",
				{"token", task.data.token},
				{
					"queryParams",
						{"id", task.data.queryParams.id}
				}
		}
	};
}

inline void from_json(const json& jason, Task2& task)
{
	jason["enunt"].get_to(task.enunt);
	jason["url"].get_to(task.url);
	jason["method"].get_to(task.method);
	jason["data"]["token"].get_to(task.data.token);
	jason["data"]["queryParams"]["id"].get_to(task.data.queryParams.id);
}

inline void to_json(json& jason, const Task3& task)
{
	jason = json
	{
		{"enunt", task.enunt},
		{"url", task.url},
		{"method", task.method}
	};
}

inline void from_json(const json& jason, Task3& task)
{
	jason["enunt"].get_to(task.enunt);
	jason["url"].get_to(task.url);
	jason["method"].get_to(task.method);
}

inline void to_json(json& jason, const Task4& task)
{
	jason = json
	{
		{"enunt", task.enunt},
		{"url", task.url},
		{"method", task.method},
		{"type", task.type},
		{
			"data",
				{"method", task.data.method},
				{"url", task.data.url},
				{
					"queryParams",
						{"q", task.data.queryParams.q},
						{"APPID", task.data.queryParams.APPID}
				}
		}
	};
}

inline void from_json(const json& jason, Task4& task)
{
	jason["enunt"].get_to(task.enunt);
	jason["url"].get_to(task.url);
	jason["method"].get_to(task.method);
	jason["type"].get_to(task.type);
	jason["data"]["method"].get_to(task.data.method);
	jason["data"]["url"].get_to(task.data.url);
	jason["data"]["queryParams"]["q"].get_to(task.data.queryParams.q);
	jason["data"]["queryParams"]["APPID"].get_to(task.data.queryParams.APPID);
}

// funcție pentru parsarea unui mesaj în format JSON
inline json parse_json(const string& message)
{
	auto start = message.find('{');

	if (start == string::npos)
		return json();

	return json::parse(message.substr(start, message.size() - start));
}