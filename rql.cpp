#include "rql.hpp"

namespace com {
	namespace rethinkdb {
		namespace driver {
		
			RQL::RQL() : RQL(com::rethinkdb::Query::QueryType::Query_QueryType_START) {};
			
			RQL::RQL(com::rethinkdb::Query::QueryType query_type) : query(com::rethinkdb::Query()) {
				this->query.set_type(query_type);

				// generate random token
				this->query.set_token(rand());
			}

			RQL* RQL::db_create(const string& name) {

				com::rethinkdb::Term *term;
				term = this->query.mutable_query();
				term->set_type(com::rethinkdb::Term::TermType::Term_TermType_DB_CREATE);

				com::rethinkdb::Term *term_name;
				term_name = term->add_args();
				term_name->set_type(com::rethinkdb::Term::TermType::Term_TermType_DATUM);

				com::rethinkdb::Datum *datum;
				datum = term_name->mutable_datum();
				datum->set_type(com::rethinkdb::Datum::DatumType::Datum_DatumType_R_STR);
				datum->set_r_str(name);

				return this;
			}

			vector<datum> RQL::run(shared_ptr<connection> conn) {
				conn->connect();

				// TODO - optargs?

				// write query
				conn->write_query(this->query);

				// read response
				shared_ptr<com::rethinkdb::Response> response(conn->read_response());

				switch (response->type()) {
				case com::rethinkdb::Response::ResponseType::Response_ResponseType_RUNTIME_ERROR:
					throw runtime_error_exception("\n\nResponse received:\n" + response->DebugString());
					break;
				case com::rethinkdb::Response::ResponseType::Response_ResponseType_CLIENT_ERROR:
					throw client_error_exception("\n\nResponse received:\n" + response->DebugString());
					break;
				case com::rethinkdb::Response::ResponseType::Response_ResponseType_COMPILE_ERROR:
					throw compile_error_exception("\n\nResponse received:\n" + response->DebugString());
					break;
				}

				// if this point is reached, response is fine
				response->PrintDebugString();

				// TODO
				return vector<datum>();
			}

		}
	}
}