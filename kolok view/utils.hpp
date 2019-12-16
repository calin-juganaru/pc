#pragma once

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <bitset>
#include <unordered_set>
#include <unordered_map>
#include <set>

using namespace std;

static constexpr auto ID_SIZE = 50u;
static constexpr auto BUFFER_SIZE = 1564u;

// extrage cuvântul din șirul dat (pană la spațiu)
string get_word(const string& cmd, size_t& offset)
{
	auto position = offset;
	while (position < cmd.size())
		if (cmd[position++] == ' ') break;

	swap(offset, position);
	return cmd.substr(position, offset - position - 1);
}

#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)
