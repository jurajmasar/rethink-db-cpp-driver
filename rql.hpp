#include "proto\ql2.pb.h"
#include "exception.hpp"
#include "connection.hpp"

#ifndef RETHINK_DB_DRIVER_RQL
#define RETHINK_DB_DRIVER_RQL

using namespace std;
using namespace com::rethinkdb;

namespace com {
	namespace rethinkdb {
		namespace driver {
			
			class RQL {
			public:

				RQL();
				RQL(Query::QueryType query_type);
				RQL::RQL(Query::QueryType query_type, size_t token);

				/* -------------------------------------------------------------------- */

				shared_ptr<Response> RQL::run(shared_ptr<connection> conn);
				shared_ptr<Response> read_more(shared_ptr<connection> conn);

				/* -------------------------------------------------------------------- */

				RQL* db(const string& db_name);
				RQL* db_create(const string& db_name);
				RQL* db_drop(const string& db_name);
				RQL* db_list();

				/* -------------------------------------------------------------------- */

				RQL* table_list();

			private:
				Query query;
				shared_ptr<Response> response;

				void add_term_datum_string(Term* term, const string& str);
			};

		}
	}
}

#endif