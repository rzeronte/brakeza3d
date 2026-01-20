#include "../../include/HttpClient/ResourceHubClient.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "../../include/Misc/Logging.h"

// Callback para escribir datos descargados a un archivo
size_t WriteFileCallback(void* ptr, size_t size, size_t nmemb, FILE* stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

// Helper para crear directorios
bool createDirectories(const std::string& path) {
#ifdef _WIN32
    std::string command = "mkdir \"" + path + "\" 2>nul";
    system(command.c_str());
#else
    std::string command = "mkdir -p \"" + path + "\" 2>/dev/null";
    system(command.c_str());
#endif
    return true;
}

ResourceHubClient::ResourceHubClient(const std::string& baseUrl) 
    : baseUrl(baseUrl), curl(nullptr) {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
}

ResourceHubClient::~ResourceHubClient() {
    if (curl) {
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

size_t ResourceHubClient::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    size_t totalSize = size * nmemb;
    userp->append((char*)contents, totalSize);
    return totalSize;
}

ResourceHubClient::HttpResponse ResourceHubClient::makeRequest(
    const std::string& method, 
    const std::string& endpoint,
    const std::string& body,
    const std::map<std::string, std::string>& additionalHeaders,
    bool includeAuth) {
    
    HttpResponse response;
    std::string readBuffer;
    
    if (!curl) {
        response.statusCode = -1;
        response.body = "CURL not initialized";
        return response;
    }
    
    std::string url = baseUrl + endpoint;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    
    // Headers
    struct curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    if (includeAuth && !accessToken.empty()) {
        std::string authHeader = "Authorization: Bearer " + accessToken;
        headers = curl_slist_append(headers, authHeader.c_str());
    }
    
    for (const auto& header : additionalHeaders) {
        std::string headerStr = header.first + ": " + header.second;
        headers = curl_slist_append(headers, headerStr.c_str());
    }
    
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    // Método y body
    if (method == "POST") {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        if (!body.empty()) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        }
    } else if (method == "DELETE") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    } else if (method == "GET") {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    }
    
    // Ejecutar request
    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        response.statusCode = -1;
        response.body = curl_easy_strerror(res);
    } else {
        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        response.statusCode = httpCode;
        response.body = readBuffer;
    }
    
    curl_slist_free_all(headers);
    
    return response;
}

ResourceHubClient::HttpResponse ResourceHubClient::uploadMultipart(
    const std::string& endpoint,
    const std::map<std::string, std::string>& fields,
    const std::map<std::string, std::string>& files) {
    
    HttpResponse response;
    std::string readBuffer;
    
    if (!curl) {
        response.statusCode = -1;
        response.body = "CURL not initialized";
        return response;
    }
    
    std::string url = baseUrl + endpoint;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    
    // Crear form
    curl_mime* form = curl_mime_init(curl);
    
    // Agregar campos
    for (const auto& field : fields) {
        curl_mimepart* part = curl_mime_addpart(form);
        curl_mime_name(part, field.first.c_str());
        curl_mime_data(part, field.second.c_str(), CURL_ZERO_TERMINATED);
    }
    
    // Agregar archivos
    for (const auto& file : files) {
        curl_mimepart* part = curl_mime_addpart(form);
        curl_mime_name(part, file.first.c_str());
        curl_mime_filedata(part, file.second.c_str());
    }
    
    curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
    
    // Headers con autenticación
    struct curl_slist* headers = nullptr;
    if (!accessToken.empty()) {
        std::string authHeader = "Authorization: Bearer " + accessToken;
        headers = curl_slist_append(headers, authHeader.c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    
    // Ejecutar
    CURLcode res = curl_easy_perform(curl);
    
    if (res != CURLE_OK) {
        response.statusCode = -1;
        response.body = curl_easy_strerror(res);
    } else {
        long httpCode = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
        response.statusCode = httpCode;
        response.body = readBuffer;
    }
    
    curl_mime_free(form);
    curl_slist_free_all(headers);
    
    return response;
}

// Autenticación

bool ResourceHubClient::login(const std::string& username, const std::string& password) {
    // Crear JSON con cJSON
    cJSON* requestJson = cJSON_CreateObject();
    cJSON_AddStringToObject(requestJson, "username", username.c_str());
    cJSON_AddStringToObject(requestJson, "password", password.c_str());
    
    char* requestBody = cJSON_PrintUnformatted(requestJson);
    cJSON_Delete(requestJson);
    
    HttpResponse response = makeRequest("POST", "/api/auth/login", requestBody, {}, false);
    free(requestBody);
    
    if (response.statusCode == 200) {
        cJSON* responseJson = cJSON_Parse(response.body.c_str());
        if (responseJson) {
            cJSON* accessTokenItem = cJSON_GetObjectItem(responseJson, "accessToken");
            cJSON* refreshTokenItem = cJSON_GetObjectItem(responseJson, "refreshToken");
            cJSON* userItem = cJSON_GetObjectItem(responseJson, "user");
            
            if (accessTokenItem && refreshTokenItem && userItem) {
                accessToken = accessTokenItem->valuestring;
                refreshToken = refreshTokenItem->valuestring;
                
                cJSON* usernameItem = cJSON_GetObjectItem(userItem, "username");
                if (usernameItem) {
                    LOG_MESSAGE("[Login] Login successful: %s", usernameItem->valuestring);
                }
                
                cJSON_Delete(responseJson);
                return true;
            }
            cJSON_Delete(responseJson);
        }
        LOG_ERROR("[Login] Error processing JSON...");
        return false;
    }

    LOG_ERROR("[Login] Login failed, StatusCode: %d", response.statusCode);
    return false;
}

bool ResourceHubClient::registerUser(const std::string& username, const std::string& email, const std::string& password) {
    cJSON* requestJson = cJSON_CreateObject();
    cJSON_AddStringToObject(requestJson, "username", username.c_str());
    cJSON_AddStringToObject(requestJson, "email", email.c_str());
    cJSON_AddStringToObject(requestJson, "password", password.c_str());
    
    char* requestBody = cJSON_PrintUnformatted(requestJson);
    cJSON_Delete(requestJson);
    
    HttpResponse response = makeRequest("POST", "/api/auth/register", requestBody, {}, false);
    free(requestBody);
    
    if (response.statusCode == 201) {
        cJSON* responseJson = cJSON_Parse(response.body.c_str());
        if (responseJson) {
            cJSON* accessTokenItem = cJSON_GetObjectItem(responseJson, "accessToken");
            cJSON* refreshTokenItem = cJSON_GetObjectItem(responseJson, "refreshToken");
            cJSON* userItem = cJSON_GetObjectItem(responseJson, "user");
            
            if (accessTokenItem && refreshTokenItem && userItem) {
                accessToken = accessTokenItem->valuestring;
                refreshToken = refreshTokenItem->valuestring;
                
                cJSON* usernameItem = cJSON_GetObjectItem(userItem, "username");
                if (usernameItem) {
                    std::cout << "Registro exitoso como: " << usernameItem->valuestring << std::endl;
                }
                
                cJSON_Delete(responseJson);
                return true;
            }
            cJSON_Delete(responseJson);
        }
        std::cerr << "Error parseando respuesta JSON" << std::endl;
        return false;
    }
    
    std::cerr << "Registro fallido (código " << response.statusCode << "): " << response.body << std::endl;
    return false;
}

void ResourceHubClient::logout() {
    if (!refreshToken.empty()) {
        cJSON* requestJson = cJSON_CreateObject();
        cJSON_AddStringToObject(requestJson, "refreshToken", refreshToken.c_str());
        
        char* requestBody = cJSON_PrintUnformatted(requestJson);
        cJSON_Delete(requestJson);
        
        makeRequest("POST", "/api/auth/logout", requestBody);
        free(requestBody);
    }
    
    accessToken.clear();
    refreshToken.clear();
    std::cout << "Logout exitoso" << std::endl;
}

bool ResourceHubClient::refreshAccessToken() {
    if (refreshToken.empty()) {
        return false;
    }
    
    cJSON* requestJson = cJSON_CreateObject();
    cJSON_AddStringToObject(requestJson, "refreshToken", refreshToken.c_str());
    
    char* requestBody = cJSON_PrintUnformatted(requestJson);
    cJSON_Delete(requestJson);
    
    HttpResponse response = makeRequest("POST", "/api/auth/refresh", requestBody, {}, false);
    free(requestBody);
    
    if (response.statusCode == 200) {
        cJSON* responseJson = cJSON_Parse(response.body.c_str());
        if (responseJson) {
            cJSON* accessTokenItem = cJSON_GetObjectItem(responseJson, "accessToken");
            if (accessTokenItem) {
                accessToken = accessTokenItem->valuestring;
                cJSON_Delete(responseJson);
                return true;
            }
            cJSON_Delete(responseJson);
        }
        std::cerr << "Error refrescando token" << std::endl;
    }
    return false;
}

// Recursos

cJSON* ResourceHubClient::getResources(int page, int limit, const std::string& search,
                                      const std::string& type, const std::string& category) {
    std::ostringstream endpoint;
    endpoint << "/api/resources?page=" << page << "&limit=" << limit;
    
    if (!search.empty()) {
        endpoint << "&search=" << search;
    }
    if (type != "all") {
        endpoint << "&type=" << type;
    }
    if (category != "all") {
        endpoint << "&category=" << category;
    }
    
    HttpResponse response = makeRequest("GET", endpoint.str(), "", {}, false);
    
    if (response.statusCode == 200) {
        cJSON* json = cJSON_Parse(response.body.c_str());
        if (!json) {
            std::cerr << "Error parseando recursos" << std::endl;
            return nullptr;
        }
        return json;  // El usuario debe hacer cJSON_Delete() después de usar
    }
    
    std::cerr << "Error obteniendo recursos (código " << response.statusCode << ")" << std::endl;
    return nullptr;
}

cJSON* ResourceHubClient::getResource(int id) {
    std::string endpoint = "/api/resources/" + std::to_string(id);
    HttpResponse response = makeRequest("GET", endpoint, "", {}, false);
    
    if (response.statusCode == 200) {
        cJSON* json = cJSON_Parse(response.body.c_str());
        if (!json) {
            std::cerr << "Error parseando recurso" << std::endl;
            return nullptr;
        }
        return json;  // El usuario debe hacer cJSON_Delete()
    }
    
    return nullptr;
}

cJSON* ResourceHubClient::getMyResources() {
    HttpResponse response = makeRequest("GET", "/api/resources/user/me");
    
    if (response.statusCode == 200) {
        cJSON* json = cJSON_Parse(response.body.c_str());
        if (!json) {
            std::cerr << "Error parseando mis recursos" << std::endl;
            return nullptr;
        }
        return json;  // El usuario debe hacer cJSON_Delete()
    }
    
    return nullptr;
}

// Upload

bool ResourceHubClient::uploadScript(const std::string& name, const std::string& description,
                                     const std::string& scriptType, const std::vector<std::string>& luaFiles) {
    std::map<std::string, std::string> fields;
    fields["name"] = name;
    fields["description"] = description;
    fields["scriptType"] = scriptType;
    
    std::map<std::string, std::string> files;
    for (const auto& file : luaFiles) {
        files["files"] = file;
    }
    
    HttpResponse response = uploadMultipart("/api/resources/scripts", fields, files);
    
    if (response.statusCode == 201) {
        std::cout << "Script subido exitosamente" << std::endl;
        return true;
    } else {
        std::cerr << "Error subiendo script (código " << response.statusCode << "): " << response.body << std::endl;
        return false;
    }
}

bool ResourceHubClient::uploadShader(const std::string& name, const std::string& description,
                                     const std::string& shaderType, const std::string& vertexFile,
                                     const std::string& fragmentFile) {
    std::map<std::string, std::string> fields;
    fields["name"] = name;
    fields["description"] = description;
    fields["shaderType"] = shaderType;
    
    std::map<std::string, std::string> files;
    files["vertexFile"] = vertexFile;
    files["fragmentFile"] = fragmentFile;
    
    HttpResponse response = uploadMultipart("/api/resources/shaders", fields, files);
    
    if (response.statusCode == 201) {
        std::cout << "Shader subido exitosamente" << std::endl;
        return true;
    } else {
        std::cerr << "Error subiendo shader (código " << response.statusCode << "): " << response.body << std::endl;
        return false;
    }
}

bool ResourceHubClient::rateResource(int resourceId, int rating) {
    if (rating < 1 || rating > 5) {
        std::cerr << "Rating debe ser entre 1 y 5" << std::endl;
        return false;
    }
    
    cJSON* requestJson = cJSON_CreateObject();
    cJSON_AddNumberToObject(requestJson, "rating", rating);
    
    char* requestBody = cJSON_PrintUnformatted(requestJson);
    cJSON_Delete(requestJson);
    
    std::string endpoint = "/api/resources/" + std::to_string(resourceId) + "/rate";
    HttpResponse response = makeRequest("POST", endpoint, requestBody);
    free(requestBody);
    
    if (response.statusCode == 200) {
        std::cout << "Recurso valorado exitosamente" << std::endl;
        return true;
    } else {
        std::cerr << "Error valorando recurso (código " << response.statusCode << "): " << response.body << std::endl;
        return false;
    }
}

// Función de descarga de archivos
bool ResourceHubClient::downloadResourceFile(int resourceId,
                                              const std::string& filename,
                                              const std::string& outputPath) {
    if (filename.empty() || outputPath.empty()) {
        std::cerr << "❌ Error: Filename y outputPath son requeridos" << std::endl;
        return false;
    }

    // Construir URL del endpoint
    std::ostringstream urlStream;
    urlStream << baseUrl << "/api/resources/" << resourceId << "/files/" << filename;
    std::string url = urlStream.str();

    std::cout << "📥 Descargando archivo..." << std::endl;
    std::cout << "   Recurso ID: " << resourceId << std::endl;
    std::cout << "   Archivo: " << filename << std::endl;
    std::cout << "   Destino: " << outputPath << std::endl;

    // Crear directorio de destino si no existe
    size_t lastSlash = outputPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        std::string directory = outputPath.substr(0, lastSlash);
        createDirectories(directory);
    }

    // Abrir archivo de salida
    FILE* outputFile = fopen(outputPath.c_str(), "wb");
    if (!outputFile) {
        std::cerr << "❌ Error: No se pudo crear el archivo de salida: " << outputPath << std::endl;
        return false;
    }

    // Configurar CURL
    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "❌ Error: No se pudo inicializar CURL" << std::endl;
        fclose(outputFile);
        return false;
    }

    // Headers
    struct curl_slist* headers = NULL;
    if (!accessToken.empty()) {
        std::string authHeader = "Authorization: Bearer " + accessToken;
        headers = curl_slist_append(headers, authHeader.c_str());
    }

    // Configurar opciones de CURL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteFileCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, outputFile);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    if (headers) {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    }

    // Realizar la petición
    CURLcode res = curl_easy_perform(curl);

    // Obtener código de respuesta HTTP
    long httpCode = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);

    // Limpiar
    if (headers) {
        curl_slist_free_all(headers);
    }
    curl_easy_cleanup(curl);
    fclose(outputFile);

    // Verificar resultado
    if (res != CURLE_OK) {
        std::cerr << "❌ Error CURL: " << curl_easy_strerror(res) << std::endl;
        remove(outputPath.c_str()); // Eliminar archivo parcial
        return false;
    }

    if (httpCode != 200) {
        std::cerr << "❌ Error HTTP " << httpCode << std::endl;

        // Leer mensaje de error del archivo
        FILE* errorFile = fopen(outputPath.c_str(), "r");
        if (errorFile) {
            char buffer[256];
            size_t bytesRead = fread(buffer, 1, sizeof(buffer) - 1, errorFile);
            buffer[bytesRead] = '\0';
            std::cerr << "   Respuesta: " << buffer << std::endl;
            fclose(errorFile);
        }

        remove(outputPath.c_str()); // Eliminar archivo de error
        return false;
    }

    // Verificar que el archivo se descargó correctamente
    FILE* checkFile = fopen(outputPath.c_str(), "rb");
    if (!checkFile) {
        std::cerr << "❌ Error: No se pudo verificar el archivo descargado" << std::endl;
        return false;
    }

    // Obtener tamaño del archivo
    fseek(checkFile, 0, SEEK_END);
    long fileSize = ftell(checkFile);
    fclose(checkFile);

    if (fileSize == 0) {
        std::cerr << "❌ Error: Archivo descargado está vacío" << std::endl;
        remove(outputPath.c_str());
        return false;
    }

    std::cout << "✅ Archivo descargado exitosamente" << std::endl;
    std::cout << "   Tamaño: " << fileSize << " bytes" << std::endl;
    std::cout << "   Ubicación: " << outputPath << std::endl;

    return true;
}
