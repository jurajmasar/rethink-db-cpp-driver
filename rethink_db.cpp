#include "rethink_db.hpp"

namespace com {
	namespace rethinkdb {
		connection::connection(const std::string& host, const std::string& port, const std::string& database, const std::string& auth_key, int timeout) : resolver_(io_service), socket_(io_service) {
			this->host = host;
			this->port = port;
			this->database = database;
			this->auth_key = auth_key;
			this->timeout = timeout;
			this->is_connected = false;
		}

		int connection::connect() {
			boost::asio::ip::tcp::resolver::query query(this->host, this->port);
			resolver_.async_resolve(query,
				boost::bind(&connection::handle_resolve, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::iterator));

			io_service.run();

			return 0;
		}

		void connection::handle_resolve(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator) {
			if (!err) {
				// Attempt a connection to each endpoint in the list until we
				// successfully establish a connection.
				boost::asio::async_connect(socket_, endpoint_iterator,
					boost::bind(&connection::handle_connect, this,
					boost::asio::placeholders::error));
			} else {
				std::cout << "Error in handle_resolve: " << err.message() << "\n";
			}
		}

		void connection::handle_connect(const boost::system::error_code& err) {
			if (!err) {
				std::ostream request_stream(&request_);

				// send magic version number
				request_stream.write((char*)&(com::rethinkdb::VersionDummy::V0_2), sizeof (com::rethinkdb::VersionDummy::V0_2));

				// send auth_key length
				u_int auth_key_length = auth_key.length();
				request_stream.write((char*)&auth_key_length, sizeof (u_int));

				// send auth_key
				request_stream.write(auth_key.c_str(), auth_key.length());

				// The connection was successful. Send the request.
				boost::asio::async_write(socket_, request_,
					boost::bind(&connection::handle_write_request, this,
					boost::asio::placeholders::error));
			} else {
				std::cout << "Error in handle_connect: " << err.message() << "\n";
			}
		}
		
		void connection::handle_write_request(const boost::system::error_code& err) {
			std::cout << "In handle_write_request" << std::endl;
			if (!err) {
				// Read the response status line. The response_ streambuf will
				// automatically grow to accommodate the entire line. The growth may be
				// limited by passing a maximum size to the streambuf constructor.
				boost::asio::async_read_until(socket_, response_, 0,
					boost::bind(&connection::handle_read_connect_response, this,
					boost::asio::placeholders::error));
			} else {
				std::cout << "Error in handle_write_request: " << err.message() << "\n";
			}
		}

		void connection::handle_read_connect_response(const boost::system::error_code& err) {
			if (!err) {
				std::istream response_stream(&response_);
				std::string response;
				std::getline(response_stream, response);
				if (response.substr(0,7) == "SUCCESS") {
					std::cout << "Successfully connected." << std::endl;
					std::cout << "Response: '" << response << "'\n";
					this->is_connected = true;
				} else {
					this->is_connected = false;
					std::cout << "Error in handle_read_connect_response: '" << response << "'\n";
				}
			} else {
				std::cout << "Error in handle_read_connect_response: " << err << "\n";
			}
		}
		
		void connection::create_db(std::string db_name) {
			com::rethinkdb::Term t = com::rethinkdb::Term();
			t.set_type(com::rethinkdb::Term::TermType::Term_TermType_DB_CREATE);

			com::rethinkdb::Term *t_name;
			t_name = t.add_args();
			t_name->set_type(com::rethinkdb::Term::TermType::Term_TermType_DATUM);
			std::cout << "Debug point." << std::endl;
			com::rethinkdb::Datum *datum;
			datum = t_name->mutable_datum();
			datum->set_type(com::rethinkdb::Datum::DatumType::Datum_DatumType_R_STR);
			datum->set_r_str(db_name);
			
			std::ostream request_stream(&request_);
			std::string blob = t.SerializeAsString();
			
			u_int blob_length = blob.length();
			request_stream.write((char*)&blob, sizeof (u_int));

			request_stream.write(blob.c_str(), blob.length());

			boost::asio::async_write(socket_, request_,
				boost::bind(&connection::handle_write_request, this,
				boost::asio::placeholders::error));
		} 
	}
}