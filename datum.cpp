#include "datum.hpp"

using namespace std;

namespace com {
	namespace rethinkdb {
		namespace driver {

			null_datum::null_datum() : datum(Datum::DatumType::Datum_DatumType_R_NULL) {};

			std::shared_ptr<array_datum> datum::to_array_datum() {
				std::shared_ptr<datum> ptr(this);
				return std::static_pointer_cast<array_datum> (ptr);
			}
			
			array_datum::array_datum() : datum(Datum::DatumType::Datum_DatumType_R_ARRAY) {
				value = std::vector<datum>();
			};

			object_datum::object_datum() : datum(Datum::DatumType::Datum_DatumType_R_OBJECT) {
				value = boost::unordered_map<std::string, datum>();
			};

		}
	}
}