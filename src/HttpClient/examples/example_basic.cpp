#include "ResourceHubClient.h"
#include <iostream>

int main() {
    // Crear cliente
    ResourceHubClient client("http://localhost:3000");
    
    std::cout << "=== Resource Hub C++ Client (cJSON) ===" << std::endl << std::endl;
    
    // 1. Login
    std::cout << "1. Intentando login..." << std::endl;
    if (client.login("demo_user", "123456")) {
        std::cout << "✓ Login exitoso!" << std::endl << std::endl;
    } else {
        std::cerr << "✗ Login fallido" << std::endl;
        return 1;
    }
    
    // 2. Obtener recursos
    std::cout << "2. Obteniendo recursos..." << std::endl;
    cJSON* resources = client.getResources(1, 10);
    
    if (resources) {
        cJSON* resourcesArray = cJSON_GetObjectItem(resources, "resources");
        
        if (resourcesArray && cJSON_IsArray(resourcesArray)) {
            int count = cJSON_GetArraySize(resourcesArray);
            std::cout << "✓ Recursos obtenidos: " << count << " recursos" << std::endl;
            
            // Mostrar primeros 3 recursos
            for (int i = 0; i < count && i < 3; i++) {
                cJSON* resource = cJSON_GetArrayItem(resourcesArray, i);
                cJSON* name = cJSON_GetObjectItem(resource, "name");
                cJSON* type = cJSON_GetObjectItem(resource, "type");
                cJSON* author = cJSON_GetObjectItem(resource, "author");
                
                if (name && type && author) {
                    std::cout << "  - " << name->valuestring 
                             << " (" << type->valuestring << ")"
                             << " por " << author->valuestring << std::endl;
                }
            }
            std::cout << std::endl;
        }
        
        cJSON_Delete(resources);  // Liberar memoria
    }
    
    // 3. Obtener recurso específico
    std::cout << "3. Obteniendo recurso #1..." << std::endl;
    cJSON* resource = client.getResource(1);
    
    if (resource) {
        cJSON* name = cJSON_GetObjectItem(resource, "name");
        cJSON* description = cJSON_GetObjectItem(resource, "description");
        cJSON* rating = cJSON_GetObjectItem(resource, "average_rating");
        
        if (name) {
            std::cout << "✓ Recurso: " << name->valuestring << std::endl;
        }
        if (description) {
            std::cout << "  Descripción: " << description->valuestring << std::endl;
        }
        if (rating) {
            std::cout << "  Rating: " << rating->valuedouble << std::endl;
        }
        std::cout << std::endl;
        
        cJSON_Delete(resource);  // Liberar memoria
    }
    
    // 4. Valorar recurso
    std::cout << "4. Valorando recurso #2..." << std::endl;
    if (client.rateResource(2, 5)) {
        std::cout << "✓ Recurso valorado con 5 estrellas" << std::endl << std::endl;
    }
    
    // 5. Obtener mis recursos
    std::cout << "5. Obteniendo mis recursos..." << std::endl;
    cJSON* myResources = client.getMyResources();
    
    if (myResources && cJSON_IsArray(myResources)) {
        int count = cJSON_GetArraySize(myResources);
        std::cout << "✓ Tienes " << count << " recursos publicados" << std::endl << std::endl;
        
        cJSON_Delete(myResources);  // Liberar memoria
    }
    
    // 6. Buscar recursos
    std::cout << "6. Buscando recursos con 'shader'..." << std::endl;
    cJSON* searchResults = client.getResources(1, 10, "shader");
    
    if (searchResults) {
        cJSON* resultsArray = cJSON_GetObjectItem(searchResults, "resources");
        
        if (resultsArray && cJSON_IsArray(resultsArray)) {
            int count = cJSON_GetArraySize(resultsArray);
            std::cout << "✓ Encontrados: " << count << " recursos" << std::endl;
            
            for (int i = 0; i < count; i++) {
                cJSON* r = cJSON_GetArrayItem(resultsArray, i);
                cJSON* name = cJSON_GetObjectItem(r, "name");
                if (name) {
                    std::cout << "  - " << name->valuestring << std::endl;
                }
            }
            std::cout << std::endl;
        }
        
        cJSON_Delete(searchResults);  // Liberar memoria
    }
    
    // 7. Logout
    std::cout << "7. Cerrando sesión..." << std::endl;
    client.logout();
    std::cout << "✓ Sesión cerrada" << std::endl;
    
    return 0;
}
