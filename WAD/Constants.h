#pragma once

namespace Biendeo {
	namespace WAD {
		typedef unsigned char byte;
		typedef unsigned int wadAddress;

		// Converts four bytes of an array into an unsigned integer.
		unsigned int LittleEndianToInt(byte* arr);

		// Converts an integer to an array of bytes.
		byte* IntToLittleEndian(unsigned int number);
		// Returns a sub-array with a null terminator at the end.
		byte* SubArray(byte* arr, int startPos, int length);
	}
}