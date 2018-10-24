#pragma once

namespace Biendeo {
	namespace WAD {
		enum class Identification {
			IWAD,
			PWAD
		};

		// Returns a readable string version of the WAD identification.
		char* IdentificationToString(Identification id);
	}
}