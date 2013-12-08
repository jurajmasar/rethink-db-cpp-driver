#include <iostream>
#include "rethink_db.cpp"

using namespace std;

int main(int argc, char* argv) {
	cout << "Hello world" << endl;

	std::shared_ptr <com::rethinkdb::connection> conn(new com::rethinkdb::connection("10.211.55.2", "28015", "test", "mojkluc"));
	std::shared_ptr <com::rethinkdb::RQL> r(new com::rethinkdb::RQL());

	try {
		r->db_create("myDB")->run(conn);
	} catch (std::runtime_error& e) {
		cerr << "An error occurred: " << e.what() << std::endl;
	}

	return 0;
}