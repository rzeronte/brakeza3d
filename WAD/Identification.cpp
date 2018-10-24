#include "Identification.h"

namespace Biendeo {
	namespace WAD {
		char* IdentificationToString(Identification type) {
			switch (type) {
				case Identification::IWAD:
					return "IWAD";
				case Identification::PWAD:
					return "PWAD";
				default:
					return nullptr;
			}
		}
	}
}