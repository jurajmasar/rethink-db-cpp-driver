#include "proto\ql2.pb.h"
#include "exception.hpp"
#include "datum.hpp"
#include "connection.hpp"

#ifndef RETHINK_DB_DRIVER_RQL
#define RETHINK_DB_DRIVER_RQL

using namespace std;

namespace com {
	namespace rethinkdb {
		namespace driver {

			class RQL {
			public:
				RQL(com::rethinkdb::Query::QueryType query_type);

				RQL();

				RQL* db_create(const std::string& name);

				std::vector<datum> run(std::shared_ptr<connection> conn);

			private:
				com::rethinkdb::Query query;

			};

		}
	}
}

#endif