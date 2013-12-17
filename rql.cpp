#include "rql.hpp"

namespace com {
	namespace rethinkdb {
		namespace driver {
		
			RQL::RQL() : RQL(Query::QueryType::Query_QueryType_START) {};
			
			RQL::RQL(Query::QueryType query_type) : query(Query()) {
				this->query.set_type(query_type);

				// generate random token
				this->query.set_token(rand());
			}

			RQL* RQL::db_create(const string& name) {

				Term *term;
				term = this->query.mutable_query();
				term->set_type(Term::TermType::Term_TermType_DB_CREATE);

				Term *term_name;
				term_name = term->add_args();
				term_name->set_type(Term::TermType::Term_TermType_DATUM);

				Datum *datum;
				datum = term_name->mutable_datum();
				datum->set_type(Datum::DatumType::Datum_DatumType_R_STR);
				datum->set_r_str(name);

				return this;
			}

			vector<datum> RQL::run(shared_ptr<connection> conn) {
				conn->connect();

				// TODO - optargs?

				// write query
				conn->write_query(this->query);

				// read response
				shared_ptr<Response> response(conn->read_response());

				switch (response->type()) {
				case Response::ResponseType::Response_ResponseType_RUNTIME_ERROR:
					throw runtime_error_exception("\n\nResponse received:\n" + response->DebugString());
					break;
				case Response::ResponseType::Response_ResponseType_CLIENT_ERROR:
					throw client_error_exception("\n\nResponse received:\n" + response->DebugString());
					break;
				case Response::ResponseType::Response_ResponseType_COMPILE_ERROR:
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