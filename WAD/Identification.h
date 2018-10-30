#pragma once

enum class Identification {
	IWAD,
	PWAD
};

// Returns a readable string version of the WAD identification.
char* IdentificationToString(Identification id);
