#include "rethink_db.hpp"

namespace com {
	namespace rethinkdb {
		connection::connection(const std::string& host, const std::string& port, const std::string& database, const std::string& auth_key) : resolver_(io_service), socket_(io_service) {
			this->host = host;
			this->port = port;
			this->database = database;
			this->auth_key = auth_key;
			this->is_connected = false;
		}

		void connection::connect() {
			try {
				boost::asio::ip::tcp::resolver::query query(this->host, this->port);
				boost::asio::ip::tcp::resolver::iterator iterator = resolver_.resolve(query);
				boost::asio::connect(socket_, iterator);

				std::ostream request_stream(&request_);

				// send magic version number
				request_stream.write((char*)&(com::rethinkdb::VersionDummy::V0_2), sizeof (com::rethinkdb::VersionDummy::V0_2));

				// send auth_key length
				u_int auth_key_length = auth_key.length();
				request_stream.write((char*)&auth_key_length, sizeof (u_int));

				// send auth_key
				request_stream.write(auth_key.c_str(), auth_key.length());

				boost::asio::write(socket_, request_);

				boost::asio::read_until(socket_, response_, 0);

				std::istream response_stream(&response_);
				std::string response;
				std::getline(response_stream, response);
				if (response.substr(0, 7) == "SUCCESS") {
					std::cout << "Successfully connected." << std::endl;
					std::cout << "Response: '" << response << "'\n";
					this->is_connected = true;
				}
				else {
					this->is_connected = false;
					std::cout << "Error in handle_read_connect_response: '" << response << "'\n";
				}			
			}
			catch (std::exception& e)
			{
				std::cerr << "Exception: " << e.what() << "\n";
			}			
		}

		
		
		void connection::create_db(std::string db_name) {
			com::rethinkdb::Query q = com::rethinkdb::Query();
			q.set_type(com::rethinkdb::Query::QueryType::Query_QueryType_START);
			q.set_token(1);

			com::rethinkdb::Term *t;
			t = q.mutable_query();
			t->set_type(com::rethinkdb::Term::TermType::Term_TermType_DB_CREATE);

			com::rethinkdb::Term *t_name;
			t_name = t->add_args();
			t_name->set_type(com::rethinkdb::Term::TermType::Term_TermType_DATUM);

			com::rethinkdb::Datum *datum;
			datum = t_name->mutable_datum();
			datum->set_type(com::rethinkdb::Datum::DatumType::Datum_DatumType_R_STR);
			datum->set_r_str(db_name);			

			std::ostream request_stream(&request_);
			std::string blob = q.SerializeAsString();
			
			u_int blob_length = blob.length();
			request_stream.write((char*)&blob_length, sizeof (u_int));

			request_stream.write(blob.c_str(), blob.length());
			try {
				boost::asio::write(socket_, request_);

				//
				// read response
				//

				// read response length
				u_int response_length;

				size_t read_length = boost::asio::read(socket_,
					boost::asio::buffer(&response_length, sizeof(u_int)));
				
				// read protobuf
				std::cout << "read_length: " << response_length << std::endl;

				//read_length = 37;

				char* reply = new char[response_length];
				size_t reply_length = boost::asio::read(socket_,
					boost::asio::buffer(reply, response_length));

				std::cout << "reply_length: " << reply_length << std::endl;


				com::rethinkdb::Response response = com::rethinkdb::Response();
				response.ParseFromArray(reply, response_length);
				delete[] reply;
				response.PrintDebugString();

			}
			catch (std::exception& e)
			{
				std::cerr << "Exception: " << e.what() << "\n";
			}


		} 
	}
}