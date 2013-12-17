#include <iostream>
#include "rethink_db.hpp"

using namespace std;
using namespace com::rethinkdb::driver;

int main(int argc, char* argv) {

	shared_ptr <connection> conn(new connection("10.211.55.2", "28015", "test", ""));
	shared_ptr <RQL> r(new RQL());

	try {
		r->db_create("myDB6")->run(conn);
	} catch (runtime_error& e) {
		cerr << e.what() << endl;
	}
	
	return 0;
}