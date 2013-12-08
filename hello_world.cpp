#include <iostream>
#include "rethink_db.cpp"

using namespace std;

int main(int argc, char* argv) {
	cout << "Hello world" << endl;

	com::rethinkdb::connection *conn = new com::rethinkdb::connection("10.211.55.2", "28015", "test", "mojkluc");
	//com::rethinkdb::RQL 
	//conn->create_db("myDB");
	delete conn;
	return 0;
}