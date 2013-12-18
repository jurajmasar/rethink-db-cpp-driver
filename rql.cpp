#include "rql.hpp"

namespace com {
	namespace rethinkdb {
		namespace driver {
		
			RQL::RQL() : RQL(Query::QueryType::Query_QueryType_START) {};
			
			RQL::RQL(Query::QueryType query_type) : RQL(query_type, rand()) {}

			RQL::RQL(Query::QueryType query_type, size_t token) : query(Query()) {
				this->query.set_type(query_type);

				// generate random token
				this->query.set_token(token);
			}

			shared_ptr<Response> RQL::run(shared_ptr<connection> conn) {
				conn->connect();

				// TODO - optargs?

				// write query
				conn->write_query(this->query);

				// read response
				response = conn->read_response();

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

				return response;
			}

			shared_ptr<Response> RQL::read_more(shared_ptr<connection> conn) {
				if (response->type() != Response::ResponseType::Response_ResponseType_SUCCESS_PARTIAL) throw connection_exception("No more data to read in the response.");

				this->query.set_type(Query::QueryType::Query_QueryType_CONTINUE);

				return this->run(conn);
			}

			/* -------------------------------------------------------------------- */

			void RQL::add_term_datum_string(Term* term, const string& str) {
				term->set_type(Term::TermType::Term_TermType_DATUM);
				term->mutable_datum()->set_type(Datum::DatumType::Datum_DatumType_R_STR);
				term->mutable_datum()->set_r_str(str);
			}

			/* -------------------------------------------------------------------- */

			RQL* RQL::db(const string& db_name) {
				this->query.mutable_query()->set_type(Term::TermType::Term_TermType_DB);
				this->add_term_datum_string(this->query.mutable_query()->add_args(), db_name);
			}

			RQL* RQL::db_create(const string& db_name) {
				this->query.mutable_query()->set_type(Term::TermType::Term_TermType_DB_CREATE);
				this->add_term_datum_string(this->query.mutable_query()->add_args(), db_name);
				return this;
			}

			RQL* RQL::db_drop(const string& db_name) {
				this->query.mutable_query()->set_type(Term::TermType::Term_TermType_DB_DROP);
				this->add_term_datum_string(this->query.mutable_query()->add_args(), db_name);
				return this;
			}

			RQL* RQL::db_list() {
				this->query.mutable_query()->set_type(Term::TermType::Term_TermType_DB_LIST);
				return this;
			}

			/* -------------------------------------------------------------------- */

			RQL* RQL::table_list() {			
				this->query.mutable_query()->set_type(Term::TermType::Term_TermType_TABLE_LIST);
				return this;
			}
		}
	}
}