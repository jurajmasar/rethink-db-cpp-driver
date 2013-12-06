#include "proto\ql2.pb.h"
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/bind/protect.hpp>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>

// fix for undefined ssize_t from https://code.google.com/p/cpp-btree/issues/detail?id=6
#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#ifndef RETHINK_DB_CONNECTION
#define RETHINK_DB_CONNECTION

namespace com {
	namespace rethinkdb {
		class connection {

			public:

				connection(const std::string& host, const std::string& port, const std::string& database, const std::string& auth_key, int timeout);
				int connect();
				void create_db(std::string db_name);
			private:

				std::string host;
				std::string port;
				std::string database;
				std::string auth_key;
				int timeout;
				int64_t token;
				bool is_connected;

				boost::asio::io_service io_service;
				boost::asio::ip::tcp::resolver resolver_;
				boost::asio::ip::tcp::socket socket_;
				boost::asio::streambuf request_;
				boost::asio::streambuf response_;

				void handle_resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
				void handle_connect(const boost::system::error_code& err);

				void handle_write_request(const boost::system::error_code& err);

				void handle_read_connect_response(const boost::system::error_code& err);

		};
	}
}

#endif