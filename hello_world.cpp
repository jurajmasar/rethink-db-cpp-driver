#include <iostream>
#include "rethink_db.hpp"

using namespace std;

int main(int argc, char* argv) {
	cout << "Hello world" << endl;

	com::rethinkdb::connection *conn = new com::rethinkdb::connection("10.211.55.2", "28015", "test", "", NULL);
	conn->connect();
//	conn->test();
	delete conn;
	return 0;
}