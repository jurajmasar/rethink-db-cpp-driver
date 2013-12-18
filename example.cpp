#include <iostream>
#include "rethink_db.hpp"

using namespace std;
using namespace com::rethinkdb;
using namespace com::rethinkdb::driver;

string ask(const string& description) {
	cout << "> " << description << ": ";
	string input = "";
	getline(cin, input);
	cout << endl;
	return input;
}

int main(int argc, char* argv) {

	shared_ptr <connection> conn(new connection("10.211.55.2", "28015", "test", ""));
	shared_ptr<Response> response = 0;

	string action;

	while (!cin.eof()) {
		try {
			cout << "Possible actions: db_create db_drop db_list exit" << endl;
			cout << endl;

			action = ask("action");

			if (action.length() == 0) {
				continue;
			}
			else if (action == "exit") {
				cout << "Exiting..." << endl << endl;
				break;
			}
			else if (action == "db_create") {
				response = (new RQL())->db_create(ask("db_name"))->run(conn);
			}
			else if (action == "db_drop") {
				response = (new RQL())->db_drop(ask("db_name"))->run(conn);
			}
			else if (action == "db_list") {
				response = (new RQL())->db_list()->run(conn);
			}

			if (response != 0) response->PrintDebugString();
		}
		catch (runtime_error& e) {
			cerr << e.what() << endl;
		}

		action = "";
		cout << endl;
		cout << "========================================================================" << endl << endl;
	}

	conn->close();

	return 0;
}