//
// Created by Eduardo on 30/12/2025.
//

#ifndef BRAKEZA3D_SHADERPREPROCESSOR_H
#define BRAKEZA3D_SHADERPREPROCESSOR_H

#pragma once
#include <string>
#include <sstream>
#include <fstream>

#include "../Misc/Logging.h"

class ShaderPreprocessor
{
public:
    /**
     * Procesa #include en un shader (un solo nivel)
     * @param shaderSource Código fuente del shader
     * @param baseDir Directorio base para buscar includes
     * @return Código procesado con includes insertados
     */
    static std::string Process(const std::string& shaderSource, const std::string& baseDir = "")
    {
        std::stringstream result;
        std::istringstream stream(shaderSource);
        std::string line;

        while (std::getline(stream, line)) {
            // Buscar #include "archivo"
            size_t includePos = line.find("#include");

            if (includePos != std::string::npos) {
                std::string includePath = ExtractPath(line);
                LOG_MESSAGE("[ShaderPreProcessor] Managing #include directive in GLSL: '%s'", includePath.c_str());
                if (!includePath.empty()) {
                    std::string fullPath = baseDir.empty() ? includePath : baseDir + "/" + includePath;
                    std::string includeContent = LoadFile(fullPath);

                    result << "// [Include: " << includePath << "]\n";
                    result << includeContent;
                } else {
                    result << line << "\n"; // Include mal formado, copiar tal cual
                }
            } else {
                result << line << "\n";
            }
        }

        return result.str();
    }

private:
    /**
     * Extrae la ruta del #include "ruta"
     */
    static std::string ExtractPath(const std::string& line)
    {
        size_t firstQuote = line.find('"');
        size_t lastQuote = line.rfind('"');

        if (firstQuote != std::string::npos && lastQuote != std::string::npos && firstQuote < lastQuote) {
            return line.substr(firstQuote + 1, lastQuote - firstQuote - 1);
        }

        return "";
    }

    /**
     * Carga el contenido de un archivo
     */
    static std::string LoadFile(const std::string& filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return "// [Error: Could not open " + filepath + "]\n";
        }

        return std::string((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
    }
};

#endif //BRAKEZA3D_SHADERPREPROCESSOR_H