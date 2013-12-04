#include "proto\ql2.pb.h"
#include <string>
#include <stdlib.h>

namespace com {
	namespace rethinkdb {
		class connection {
			public:
				connection(std::string host, unsigned short port, std::string database, std::string auth_key, int timeout) {
					this->host = host;
					this->port = port;
					this->database = database;
					this->auth_key = auth_key;
					this->timeout = timeout;
				}

				int connect() {

					if (rdb_conn->s >= 0) return 0;

					struct sockaddr_in sin;
					memset(&sin, 0, sizeof(sin));

					rdb_conn->s = socket(AF_INET, SOCK_STREAM, 0);
					if (rdb_conn->s < 0) {
						return -1;
					}

					sin.sin_family = AF_INET;
					sin.sin_addr.s_addr = inet_addr(rdb_conn->addr);
					sin.sin_port = htons(rdb_conn->port);

					if (connect(rdb_conn->s, (struct sockaddr *) &sin, sizeof(sin))) {
						close(rdb_conn->s);
						rdb_conn->s = -1;
						return -1;
					}

					return 0;

				}

				int send(void *buf, size_t len) {
					size_t remains = len;
					void *ptr = buf;
					while (remains) {
						ssize_t rlen = write(rdb_conn->s, ptr, remains);
						if (rlen <= 0) {
							return -1;
						}
						ptr += rlen;
						remains -= rlen;
					}
					return 0;
				}

			private:
				std::string host;
				unsigned short port;
				std::string database;
				std::string auth_key;
				int timeout;
				bool version_sent = false;

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