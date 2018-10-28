#pragma once

#include <vector>

#include "Constants.h"
#include "Directory.h"
#include "Identification.h"
#include "Signature.h"

namespace Biendeo {
	namespace WAD {
		// The base WAD class, it should contain all the information of a WAD.
		class WAD {
			public:
			// Creates a new WAD from the given WAD path.
			WAD(char* wadLocation);
			// Destroys the WAD object.
			~WAD();

			// Writes this object to a given file location.
			bool Write(char* wadLocation);
			bool Testing();
            int getIndexLumpByName(std::string lump_name);
            void parseLINEDEFS(Directory );
            void parseVERTEXES(Directory );

			private:
			// The signature of this WAD.
			Signature signature;
			// The directory?
			// TODO: Figure out what this is.
			std::vector<Directory> directories;

			// Reads a file an returns an array of its contents.
			byte* ReadFile(char* fileLocation);

			void DoomPicture(wadAddress);

            };
	}
}