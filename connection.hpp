#include "proto\ql2.pb.h"
#include <boost/asio.hpp>
#include <string>
#include <boost/format.hpp>
#include "exception.hpp"


#ifndef RETHINK_DB_DRIVER_CONNECTION
#define RETHINK_DB_DRIVER_CONNECTION

using namespace std;
using namespace boost::asio;

namespace com {
	namespace rethinkdb {
		namespace driver {

			//
			// implementation of connection class
			//

			class connection {

			public:

				connection(const std::string& host, const std::string& port, const std::string& database, const std::string& auth_key);

				bool connect();

				std::shared_ptr<com::rethinkdb::Response> read_response();

				void write_query(const com::rethinkdb::Query& query);

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
			};

		

		}
	}
}
#endif