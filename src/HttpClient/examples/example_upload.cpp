#include "ResourceHubClient.h"
#include <iostream>
#include <fstream>

// Helper para crear archivos de ejemplo
void createExampleLuaFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    file << content;
    file.close();
}

void createExampleShaderFile(const std::string& filename, const std::string& content) {
    std::ofstream file(filename);
    file << content;
    file.close();
}

int main() {
    ResourceHubClient client("http://localhost:3000");
    
    std::cout << "=== Resource Hub C++ Client - Upload Example (cJSON) ===" << std::endl << std::endl;
    
    // Login
    std::cout << "Iniciando sesión..." << std::endl;
    if (!client.login("demo_user", "123456")) {
        std::cerr << "Error: No se pudo iniciar sesión" << std::endl;
        return 1;
    }
    std::cout << "✓ Sesión iniciada" << std::endl << std::endl;
    
    // Ejemplo 1: Subir un script
    std::cout << "=== Ejemplo 1: Subir Script ===" << std::endl;
    
    // Crear archivos .lua de ejemplo
    createExampleLuaFile("test_script.lua", R"(
-- Test Script
function update(dt)
    print("Update called with dt: " .. dt)
end

function draw()
    print("Draw called")
end
)");
    
    std::cout << "Archivo test_script.lua creado" << std::endl;
    
    // Subir script
    std::vector<std::string> luaFiles = {"test_script.lua"};
    
    bool scriptUploaded = client.uploadScript(
        "Test Script desde C++",
        "Este es un script de prueba subido desde el cliente C++ con cJSON",
        "local",
        luaFiles
    );
    
    if (scriptUploaded) {
        std::cout << "✓ Script subido exitosamente!" << std::endl;
    } else {
        std::cout << "✗ Error subiendo script" << std::endl;
    }
    std::cout << std::endl;
    
    // Ejemplo 2: Subir un shader
    std::cout << "=== Ejemplo 2: Subir Shader ===" << std::endl;
    
    // Crear archivos de shader de ejemplo
    createExampleShaderFile("test.vert", R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}
)");
    
    createExampleShaderFile("test.frag", R"(
#version 330 core
out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec3 color;

void main() {
    FragColor = texture(texture1, TexCoord) * vec4(color, 1.0);
}
)");
    
    std::cout << "Archivos test.vert y test.frag creados" << std::endl;
    
    // Subir shader
    bool shaderUploaded = client.uploadShader(
        "Test Shader desde C++",
        "Este es un shader de prueba subido desde el cliente C++ con cJSON",
        "object",
        "test.vert",
        "test.frag"
    );
    
    if (shaderUploaded) {
        std::cout << "✓ Shader subido exitosamente!" << std::endl;
    } else {
        std::cout << "✗ Error subiendo shader" << std::endl;
    }
    std::cout << std::endl;
    
    // Verificar recursos subidos
    std::cout << "=== Verificando recursos subidos ===" << std::endl;
    cJSON* myResources = client.getMyResources();
    
    if (myResources && cJSON_IsArray(myResources)) {
        int count = cJSON_GetArraySize(myResources);
        std::cout << "Tienes " << count << " recursos:" << std::endl;
        
        for (int i = 0; i < count; i++) {
            cJSON* resource = cJSON_GetArrayItem(myResources, i);
            cJSON* name = cJSON_GetObjectItem(resource, "name");
            cJSON* type = cJSON_GetObjectItem(resource, "type");
            
            if (name && type) {
                std::cout << "  - " << name->valuestring 
                         << " (" << type->valuestring << ")" << std::endl;
            }
        }
        
        cJSON_Delete(myResources);  // Liberar memoria
    }
    std::cout << std::endl;
    
    // Buscar recursos propios
    std::cout << "=== Buscando recursos con 'C++' ===" << std::endl;
    cJSON* searchResults = client.getResources(1, 10, "C++");
    
    if (searchResults) {
        cJSON* resultsArray = cJSON_GetObjectItem(searchResults, "resources");
        
        if (resultsArray && cJSON_IsArray(resultsArray)) {
            int count = cJSON_GetArraySize(resultsArray);
            std::cout << "Encontrados " << count << " recursos:" << std::endl;
            
            for (int i = 0; i < count; i++) {
                cJSON* resource = cJSON_GetArrayItem(resultsArray, i);
                cJSON* name = cJSON_GetObjectItem(resource, "name");
                
                if (name) {
                    std::cout << "  - " << name->valuestring << std::endl;
                }
            }
        }
        
        cJSON_Delete(searchResults);  // Liberar memoria
    }
    std::cout << std::endl;
    
    // Logout
    client.logout();
    
    std::cout << "¡Ejemplo completado!" << std::endl;
    std::cout << "Los archivos subidos están en el servidor en backend/uploads/" << std::endl;
    
    return 0;
}
