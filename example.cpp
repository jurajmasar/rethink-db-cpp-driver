#include <iostream>
#include <vector>
#include <algorithm>
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
	vector<shared_ptr<Response>> responses = vector<shared_ptr<Response>>();
	
	string action;

	while (!cin.eof()) {
		try {
			cout << "Possible actions: db_create db_drop db_list table_list exit" << endl;
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
				responses = conn->r()->db_create(ask("db_name"))->run();
			}
			else if (action == "db_drop") {
				responses = conn->r()->db_drop(ask("db_name"))->run();
			}
			else if (action == "db_list") {
				responses = conn->r()->db_list()->run();
			}
			else if (action == "table_list") {
				responses = conn->r()->db(ask("db_name"))->table_list()->run();
			}
			else {
				cout << "Invalid action." << endl << endl;
				responses = vector<shared_ptr<Response>>();
			}

			for_each(responses.begin(), responses.end(), [] (shared_ptr<Response> response) {
				response->PrintDebugString();
			});
			
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

// conn->r()->db_create(ask("db_name"))->run();