#include "proto\ql2.pb.h"
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <stdlib.h>
#include <stdexcept>
#include <boost/format.hpp>
#include <vector>
#include <boost/unordered_map.hpp>

// fix for undefined ssize_t from https://code.google.com/p/cpp-btree/issues/detail?id=6
#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#ifndef RETHINK_DB_DRIVER
#define RETHINK_DB_DRIVER
namespace com {
	namespace rethinkdb {
		namespace driver {
			//
			// definitions of exceptions
			//

			class connection_exception : public std::runtime_error{
			public:
				connection_exception() :runtime_error(""){}
				connection_exception(const std::string& msg) : runtime_error(("RethinkDB connection exception. " + msg).c_str()){}
			};

			class runtime_error_exception : public std::runtime_error {
			public:
				runtime_error_exception() : runtime_error(""){}
				runtime_error_exception(const std::string& msg) : runtime_error(("RethinkDB runtime exception. " + msg).c_str()){}
			};

			class compile_error_exception : public std::runtime_error {
			public:
				compile_error_exception() :runtime_error(""){}
				compile_error_exception(const std::string& msg) : runtime_error(("RethinkDB compile error exception. " + msg).c_str()){}
			};

			class client_error_exception : public std::runtime_error {
			public:
				client_error_exception() :runtime_error(""){}
				client_error_exception(const std::string& msg) : runtime_error(("RethinkDB client error exception. " + msg).c_str()){}
			};
		}
	}
}
#endif