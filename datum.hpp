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

			class datum {
			public:
				Datum::DatumType type;
				datum(Datum::DatumType t) : type(t) {};

				shared_ptr<array_datum> to_array_datum();
			};


			class null_datum {
			public:
				null_datum();
			};

			class bool_datum : datum {
			public:
				bool value;
				bool_datum(bool v) : datum(Datum::DatumType::Datum_DatumType_R_BOOL), value(v) {};
			};

			class num_datum : datum {
			public:
				double value;
				num_datum(double v) : datum(Datum::DatumType::Datum_DatumType_R_NUM), value(v) {};
			};

			class str_datum : datum {
			public:
				string value;
				str_datum(string v) : datum(Datum::DatumType::Datum_DatumType_R_STR), value(v) {};
			};

			class array_datum : datum {
			public:
				vector<datum> value;
				array_datum();
			};

			class object_datum : datum {
			public:
				boost::unordered_map<string, datum> value;
				object_datum();
			};

		}
	}
}
#endif