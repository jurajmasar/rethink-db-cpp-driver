#include <iostream>
#include "rethink_db.cpp"

using namespace std;

int main(int argc, char* argv) {
	cout << "Hello world" << endl;

	com::rethinkdb::connection conn = com::rethinkdb::connection("10.211.55.2", "28015", "test", "", NULL);
	conn.connect();

	return 0;
}