#pragma once

#include "Constants.h"
#include "Identification.h"

namespace Biendeo {
	namespace WAD {
		// The signature of the WAD, which contains the main information.
		struct Signature {
			// Creates a default signature with 0 properties.
			Signature();
			// Fills out the signature's properties based on a given binary array.
			// This array must start at the signature start.
			Signature(byte* wadBinary);

			// The type of WAD this is (IWAD or WAD).
			Identification wadType;
			// The number of lumps in this WAD.
			unsigned int numLumps;
			// The location of the directory.
			// TODO: Figure out what this is.
			wadAddress infoTableOfS;

			// Converts this signature into the byte format.
			byte* ToBytes();
		};
	}
}