// Translation unit unica para la implementacion de miniaudio (single-header lib).
// El resto del proyecto solo hace #include "miniaudio.h" (modo declaracion), este
// .cpp es el UNICO sitio donde se define MINIAUDIO_IMPLEMENTATION -- si se define en
// mas de una TU, el linker falla por simbolos duplicados.
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
