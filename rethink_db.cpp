#include "rethink_db.hpp"

namespace com {
	namespace rethinkdb {
		namespace driver {
			

			//
			// implementation of connection class
			//

			class connection {

			public:

				connection(const std::string& host, const std::string& port, const std::string& database, const std::string& auth_key) : resolver_(io_service), socket_(io_service) {
					this->host = host;
					this->port = port;
					this->database = database;
					this->auth_key = auth_key;
					this->is_connected = false;
				}

				bool connect() {

					if (socket_.is_open() && this->is_connected) return true;

					std::string response;

					try {
						// resolve the host
						boost::asio::ip::tcp::resolver::query query(this->host, this->port);
						boost::asio::ip::tcp::resolver::iterator iterator = this->resolver_.resolve(query);
						boost::asio::connect(this->socket_, iterator);

						// prepare stream for writing data
						std::ostream request_stream(&(this->request_));

						// write magic version number
						request_stream.write((char*)&(com::rethinkdb::VersionDummy::V0_2), sizeof (com::rethinkdb::VersionDummy::V0_2));

						// write auth_key length
						u_int auth_key_length = this->auth_key.length();
						request_stream.write((char*)&auth_key_length, sizeof (u_int));

						// write auth_key
						request_stream.write(this->auth_key.c_str(), auth_key.length());

						// send the request
						boost::asio::write(this->socket_, this->request_);

						// read response until a null character 
						boost::asio::read_until(this->socket_, this->response_, 0);

						// prepare to read a response
						std::istream response_stream(&(this->response_));						

						// read one line of response
						std::getline(response_stream, response);
					}
					catch (std::exception& e)
					{
						// exception from boost has been caught
						throw connection_exception(std::string(e.what()));
					}

					// if it starts with "SUCCESS"
					if (response.substr(0, 7) == "SUCCESS") {
						this->is_connected = true;
						return true;
					}
					else {
						this->is_connected = false;
						throw connection_exception(response);
					}
					
				}

				std::shared_ptr<com::rethinkdb::Response> read_response() {
					u_int response_length;
					char* reply;
					size_t bytes_read;
					std::shared_ptr<com::rethinkdb::Response> response(new com::rethinkdb::Response());

					try {
						try {
							// read length of the response
							boost::asio::read(this->socket_, boost::asio::buffer(&response_length, sizeof(u_int)));

							// read content
							reply = new char[response_length];
							bytes_read = boost::asio::read(this->socket_, boost::asio::buffer(reply, response_length));
						}
						catch (std::exception&) {
							throw connection_exception("Unable to read from the socket.");
						}

						if (bytes_read != response_length) throw connection_exception(boost::str(boost::format("%1% bytes read, when %2% bytes promised.") % bytes_read % response_length));

						try {
							response->ParseFromArray(reply, response_length);
						}
						catch (std::exception&) {
							throw connection_exception("Unable to parse the protobuf Response object.");
						}

						delete[] reply;
					}
					catch (std::exception& e) {
						throw connection_exception(boost::str(boost::format("Read response: %1%") % e.what()));
					}

					return response;
				}

				void write_query(const com::rethinkdb::Query& query) {
					// prepare output stream
					std::ostream request_stream(&request_);

					// serialize query
					std::string blob = query.SerializeAsString();

					// write blob's length in little endian 32 bit unsigned integer
					u_int blob_length = blob.length();
					request_stream.write((char*)&blob_length, sizeof (u_int));

					// write protobuf blob
					request_stream.write(blob.c_str(), blob.length());

					try {
						// send the content of the output stream over the wire
						boost::asio::write(this->socket_, this->request_);
					}
					catch (std::exception& e)
					{
						throw connection_exception(boost::str(boost::format("Write query: exception: %1%") % e.what()));
					}
				}

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

			//
			// implementation data structures			
			//

			class array_datum;
			class null_datum;
			class bool_datum;
			class num_datum;
			class str_datum;
			class array_datum;
			class object_datum;

			class datum {
			public:
				com::rethinkdb::Datum::DatumType type;
				datum(com::rethinkdb::Datum::DatumType t) : type(t) {};

				std::shared_ptr<array_datum> to_array_datum() {
					std::shared_ptr<datum> ptr(this);
					return std::static_pointer_cast<array_datum> (ptr);
				}
			};
			

			class null_datum {
			public:
				null_datum();
			};
			
			class bool_datum : datum {
			public:
				bool value;
				bool_datum(bool v) : datum(com::rethinkdb::Datum::DatumType::Datum_DatumType_R_BOOL), value(v) {};
			};

			class num_datum : datum {
			public:
				double value;
				num_datum(double v) : datum(com::rethinkdb::Datum::DatumType::Datum_DatumType_R_NUM), value(v) {};
			};

			class str_datum : datum {
			public:
				std::string value;
				str_datum(std::string v) : datum(com::rethinkdb::Datum::DatumType::Datum_DatumType_R_STR), value(v) {};
			};

			class array_datum : datum {
			public:
				std::vector<datum> value;
				array_datum() : datum(com::rethinkdb::Datum::DatumType::Datum_DatumType_R_ARRAY) {
					value = std::vector<datum>();
				};
			};

			class object_datum : datum {
			public:
				boost::unordered_map<std::string, datum> value;
				object_datum() : datum(com::rethinkdb::Datum::DatumType::Datum_DatumType_R_OBJECT) {
					value = boost::unordered_map<std::string, datum>();
				};
			}; 							

			
			/*const com::rethinkdb::driver::datum& parse(const com::rethinkdb::Datum& input) {
				/*std::shared_ptr<datum> output;
				
				switch (input.type()) {
				case com::rethinkdb::Datum::DatumType::Datum_DatumType_R_NULL:
					output = std::make_shared<null_datum>(null_datum());
					break;
				case com::rethinkdb::Datum::DatumType::Datum_DatumType_R_BOOL:
					output = std::make_shared<bool_datum>(bool_datum(input.r_bool()));
					break;
				case com::rethinkdb::Datum::DatumType::Datum_DatumType_R_NUM:
					output = std::make_shared<num_datum>(num_datum(input.r_num()));
					break;
				case com::rethinkdb::Datum::DatumType::Datum_DatumType_R_STR:
					output = std::make_shared<str_datum>(str_datum(input.r_str()));
					break;
				case com::rethinkdb::Datum::DatumType::Datum_DatumType_R_ARRAY:
					output = std::make_shared<array_datum>(array_datum());
					for (int i = 0, s = input.r_array_size(); i < s; i++) {
						//output->to_array_datum()->value.push_back(parse(input.r_array(i)));
						//parse(&input.r_array(i));
						//output.push_back(t);
					}
					break;
				case com::rethinkdb::Datum::DatumType::Datum_DatumType_R_OBJECT:
					// TODO
					break;
				}

				return output;
				
				return datum(com::rethinkdb::Datum::DatumType::Datum_DatumType_R_NULL);
			}*/
			

			//
			// implementation of RQL class				
			//

			class RQL {
			public:
				RQL(com::rethinkdb::Query::QueryType query_type) : query(com::rethinkdb::Query()) {
					this->query.set_type(query_type);

					// generate random token
					this->query.set_token(rand());
				}

				RQL() : RQL(com::rethinkdb::Query::QueryType::Query_QueryType_START) {};

				RQL* db_create(const std::string& name) {

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

				std::vector<datum> run(std::shared_ptr<connection> conn) {
					conn->connect();

					// TODO - optargs?

					// write query
					conn->write_query(this->query);

					// read response
					std::shared_ptr<com::rethinkdb::Response> response(conn->read_response());

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



				}

			private:
				com::rethinkdb::Query query;

			};


		}
	}
}