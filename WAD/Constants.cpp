#include "Constants.h"

#include <memory>

unsigned int LittleEndianToInt(byte* arr) {
	return (arr[0] | arr[1] << 8 | arr[2] << 16 | arr[3] << 24);
}

byte* IntToLittleEndian(unsigned int number) {
	byte* returnArr = new byte[4];
	returnArr[0] = (byte)number;
	returnArr[1] = (byte)(number >> 8);
	returnArr[2] = (byte)(number >> 16);
	returnArr[3] = (byte)(number >> 24);

	return returnArr;
}

byte* SubArray(byte* arr, int startPos, int length) {
	byte* returnArr = new byte[length + 1];
	memcpy(returnArr, &arr[startPos], length);
	returnArr[length] = '\0';
	return returnArr;
}
