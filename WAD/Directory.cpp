#include "Directory.h"

#include <memory>

namespace Biendeo {
	namespace WAD {
		Directory::Directory() {
			offData = 0;
			sizeData = 0;
			std::fill(&name[0], &name[7], '\0');
		}

		Directory::Directory(byte* wadBinary) {
			offData = (wadAddress)LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(wadBinary, 0, 4)).get());
			sizeData = LittleEndianToInt(std::unique_ptr<byte[]>(SubArray(wadBinary, 4, 4)).get());

			memcpy(name, (char*)std::unique_ptr<byte[]>(SubArray(wadBinary, 8, 8)).get(), 8);

		}

		byte* Directory::ToBytes() {
			byte* arr = new byte[16];

			memcpy(&arr[0], std::unique_ptr<byte[]>(IntToLittleEndian(offData)).get(), 4);
			memcpy(&arr[4], std::unique_ptr<byte[]>(IntToLittleEndian(sizeData)).get(), 4);
			memcpy(&arr[8], name, 8);

			return arr;
		}

		void Directory::consoleInfo()
		{
			printf("Directory Name: %s | Size: %d\r\n", this->name, this->sizeData);
		}
	}
}