#include "proto\ql2.pb.h"
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <string.h>
#include <stdlib.h>

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

					std::ostream request_stream(&request_);
				}

				int connect() {
					io_service.run();

					boost::asio::ip::tcp::resolver::query query(this->host, this->port);
					resolver_.async_resolve(query,
						boost::bind(&client::handle_resolve, this,
						boost::asio::placeholders::error,
						boost::asio::placeholders::iterator));
					return 0;

				}

				int send(void *buf, size_t len) {
					/*
					size_t remains = len;
					void *ptr = buf;
					while (remains) {
						ssize_t rlen = write(sock, ptr, remains);
						if (rlen <= 0) {
							return -1;
						}
						ptr += rlen;
						remains -= rlen;
					}
					*/
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

				int send_version() {
					if (version_sent) return 0;

					if (send("\x35\xba\x61\xaf", 4) == 0) {
						version_sent = 1;
						return 0;
					}

					return -1;
				}
		};
	}
}

#endif