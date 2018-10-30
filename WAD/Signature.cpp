#include "Signature.h"

#include <memory>
#include <string>

Signature::Signature() {
	wadType = Identification::IWAD;
	numLumps = 0;
	infoTableOfS = 0;
}

Signature::Signature(byte* wadBinary) {
	std::string wadTypeStr((char*)std::unique_ptr<byte[]>(SubArray(wadBinary, 0, 4)).get());

	if (wadTypeStr == "IWAD") {
		wadType = Identification::IWAD;
	} else if (wadTypeStr == "PWAD") {
		wadType = Identification::PWAD;
	} else {
		return;
	}

	numLumps = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(wadBinary, 4, 4)).get());
	infoTableOfS = (wadAddress)LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(wadBinary, 8, 4)).get());
}

byte* Signature::ToBytes() {
	// This creates a new array for the content. It is always 12 bytes long.
	byte* arr = new byte[12];

	memcpy(&arr[0], IdentificationToString(wadType), 4);
	memcpy(&arr[4], std::unique_ptr<byte[]>(IntToLittleEndian(numLumps)).get(), 4);
	memcpy(&arr[8], std::unique_ptr<byte[]>(IntToLittleEndian(infoTableOfS)).get(), 4);

	return arr;
}
