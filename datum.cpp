#include "datum.hpp"

using namespace std;

namespace com {
	namespace rethinkdb {
		namespace driver {

			shared_ptr<array_datum> datum::to_array_datum() {
				shared_ptr<datum> ptr(this);
				return static_pointer_cast<array_datum> (ptr);
			}
			
			array_datum::array_datum() : datum(Datum::DatumType::Datum_DatumType_R_ARRAY) {
				value = vector<datum>();
			};

			object_datum::object_datum() : datum(Datum::DatumType::Datum_DatumType_R_OBJECT) {
				value = boost::unordered_map<string, datum>();
			};

		}
	}
}