#include "proto\ql2.pb.h"
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <string.h>
#include <stdlib.h>
#include <bitset>

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
				connection(const std::string & host, const std::string & port, const std::string & database, const std::string & auth_key, int timeout) : resolver_(io_service), socket_(io_service) {
					this->host = host;
					this->port = port;
					this->database = database;
					this->auth_key = auth_key;
					this->timeout = timeout;
				}

				int connect() {
					std::cout << "Debug: In connect()..." << std::endl;
					boost::asio::ip::tcp::resolver::query query(this->host, this->port);
					resolver_.async_resolve(query,
						boost::bind(&connection::handle_resolve, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::iterator));

					io_service.run();

					return 0;
				}

			private:

				std::string host;
				std::string port;
				std::string database;
				std::string auth_key;
				int timeout;
				bool version_sent = false;
				
				boost::asio::io_service io_service;
				boost::asio::ip::tcp::resolver resolver_;
				boost::asio::ip::tcp::socket socket_;
				boost::asio::streambuf request_;
				boost::asio::streambuf response_; 

				int64_t token;

				/*
				int send_version() {
					if (version_sent) return 0;

					if (send("\x35\xba\x61\xaf", 4) == 0) {
						version_sent = 1;
						return 0;
					}

					return -1;
				}
				*/
				void handle_resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator)
				{
					std::cout << "Debug: In handle_resolve..." << std::endl;
					if (!err)
					{
						// Attempt a connection to each endpoint in the list until we
						// successfully establish a connection.
						boost::asio::async_connect(socket_, endpoint_iterator,
							boost::bind(&connection::handle_connect, this,
							boost::asio::placeholders::error));
					}
					else
					{
						std::cout << "Error1: " << err.message() << "\n";
					}
				}

				void handle_connect(const boost::system::error_code& err)
				{
					std::cout << "Debug: In handle_connect..." << std::endl;
					if (!err)
					{
						std::ostream request_stream(&request_);

						u_int x = com::rethinkdb::VersionDummy::V0_2;
						request_stream.write((char*)&x, sizeof (u_int));

						x = auth_key.length();
						request_stream.write((char*)&x, sizeof (u_int));

						request_stream.write(auth_key.c_str(), auth_key.length());

						/*
						std::bitset<30> x(0);
						std::string s = x.to_string();
						request_stream.write(s.c_str(), s.length());
						*/

						
						// send magic version number
						/*request_stream.put(com::rethinkdb::VersionDummy::V0_2);
						
						// send auth_key
						request_stream.put(auth_key.length());
						request_stream << auth_key;
						*/

						// The connection was successful. Send the request.
						boost::asio::async_write(socket_, request_,
							boost::bind(&connection::handle_write_request, this,
							boost::asio::placeholders::error));
					}
					else
					{
						std::cout << "Error2: " << err.message() << "\n";
					}
				}

				void handle_write_request(const boost::system::error_code& err)
				{
					std::cout << "Debug: In handle_write_request..." << std::endl;
					if (!err)
					{
						// Read the response status line. The response_ streambuf will
						// automatically grow to accommodate the entire line. The growth may be
						// limited by passing a maximum size to the streambuf constructor.
						boost::asio::async_read_until(socket_, response_, 0,
							boost::bind(&connection::handle_read_status_line, this,
							boost::asio::placeholders::error));
					}
					else
					{
						std::cout << "Error3: " << err.message() << "\n";
					}
				}

				void handle_read_status_line(const boost::system::error_code& err)
				{
					std::cout << "Debug: In handle_read_status_line..." << std::endl;
					if (!err) // if (!err)
					{
						// Check that response is OK.

						std::istream response_stream(&response_);
						std::string response;
						response_stream >> response;
						std::cout << "Response: " << response << std::endl;
						/*
						std::istream response_stream(&response_);
						std::string response;
						std::getline(response_stream,response);
						std::cout << response << std::endl;
						*/
					}
					else
					{
						std::cout << "Error4: " << err << "\n";
					}
				}

		};
	}
}

#endif