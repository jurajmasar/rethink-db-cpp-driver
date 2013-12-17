#include "proto\ql2.pb.h"
#include <boost/unordered_map.hpp>
#include <string>

#ifndef RETHINK_DB_DRIVER_DATUM
#define RETHINK_DB_DRIVER_DATUM

using namespace std;
using namespace com::rethinkdb;

namespace com {
	namespace rethinkdb {
		namespace driver {

			class array_datum;
			class object_datum;

			class datum {
			public:
				Datum::DatumType type;
				datum(Datum::DatumType t) : type(t) {};

				shared_ptr<array_datum> to_array_datum();

				shared_ptr<object_datum> to_object_datum();
			};


			class null_datum : public datum {
			public:
				null_datum() : datum(Datum::DatumType::Datum_DatumType_R_NULL) {};
			};

			class bool_datum : public datum {
			public:
				bool value;
				bool_datum(bool v) : datum(Datum::DatumType::Datum_DatumType_R_BOOL), value(v) {};
			};

			class num_datum : public datum {
			public:
				double value;
				num_datum(double v) : datum(Datum::DatumType::Datum_DatumType_R_NUM), value(v) {};
			};

			class str_datum : public datum {
			public:
				string value;
				str_datum(string v) : datum(Datum::DatumType::Datum_DatumType_R_STR), value(v) {};
			};

			class array_datum : public datum {
			public:
				vector<shared_ptr<datum>> value;
				array_datum();
			};

			class object_datum : public datum {
			public:
				boost::unordered_map<string, shared_ptr<datum>> value;
				object_datum();
			};

		}
	}
}
#endif