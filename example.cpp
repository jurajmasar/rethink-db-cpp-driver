#include <iostream>
#include "rethink_db.hpp"

using namespace std;
using namespace com::rethinkdb;
using namespace com::rethinkdb::driver;

int main(int argc, char* argv) {

	shared_ptr <connection> conn(new connection("10.211.55.2", "28015", "test", ""));
	shared_ptr <RQL> r(new RQL());

	try {
		shared_ptr<Response> response = r->db_create("myDB110")->run(conn);

		cout << response->response(0).r_object(0).key() << ":" << response->response(0).r_object(0).val().r_num() << endl;

	} catch (runtime_error& e) {
		cerr << e.what() << endl;
	}
	
	return 0;
}