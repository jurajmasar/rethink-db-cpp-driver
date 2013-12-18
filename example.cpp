#include <iostream>
#include "rethink_db.hpp"

using namespace std;
using namespace com::rethinkdb;
using namespace com::rethinkdb::driver;

string ask(const string& description) {
	cout << "> " << description << ": ";
	string input;
	getline(cin, input);
	cout << endl;
	return input;
}

int main(int argc, char* argv) {

	shared_ptr <connection> conn(new connection("10.211.55.2", "28015", "test", ""));
	shared_ptr<Response> response;

	while (!cin.eof()) {
		try {
			cout << "Possible actions: db_create db_drop exit" << endl;
			cout << endl;

			string action = ask("action");

			if (action == "exit") {
				cout << "Exiting..." << endl << endl;
				break;
			}
			else if (action == "db_create") {
				response = (new RQL())->db_create(ask("db_name"))->run(conn);
			}
			else if (action == "db_drop") {
				response = (new RQL())->db_drop(ask("db_name"))->run(conn);
			}

			response->PrintDebugString();
		}
		catch (runtime_error& e) {
			cerr << e.what() << endl;
		}

		cout << endl;
		cout << "========================================================================" << endl << endl;
	}

		//cout << response->response(0).r_object(0).key() << ":" << response->response(0).r_object(0).val().r_num() << endl;

	
	return 0;
}