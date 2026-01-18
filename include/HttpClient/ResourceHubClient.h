#ifndef RESOURCE_HUB_CLIENT_H
#define RESOURCE_HUB_CLIENT_H

#include <string>
#include <vector>
#include <map>
#include <curl/curl.h>
#include "../Misc/cJSON.h"

class ResourceHubClient {
private:
    std::string baseUrl;
    std::string accessToken;
    std::string refreshToken;
    CURL* curl;
    
    // Callback para recibir datos
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    
    // Helper para hacer requests HTTP
    struct HttpResponse {
        int statusCode;
        std::string body;
        std::map<std::string, std::string> headers;
    };
    
    HttpResponse makeRequest(const std::string& method, const std::string& endpoint, 
                            const std::string& body = "", 
                            const std::map<std::string, std::string>& headers = {},
                            bool includeAuth = true);
    
    // Helper para multipart/form-data (upload de archivos)
    HttpResponse uploadMultipart(const std::string& endpoint, 
                                 const std::map<std::string, std::string>& fields,
                                 const std::map<std::string, std::string>& files);

public:
    ResourceHubClient(const std::string& baseUrl = "http://localhost:3000");
    ~ResourceHubClient();
    
    // Autenticación
    bool login(const std::string& username, const std::string& password);
    bool registerUser(const std::string& username, const std::string& email, const std::string& password);
    void logout();
    bool refreshAccessToken();
    
    // Recursos
    cJSON* getResources(int page = 1, int limit = 9, const std::string& search = "",
                       const std::string& type = "all", const std::string& category = "all");
    cJSON* getResource(int id);
    cJSON* getMyResources();
    
    // Upload de recursos
    bool uploadScript(const std::string& name, const std::string& description,
                     const std::string& scriptType, const std::vector<std::string>& luaFiles);
    bool uploadShader(const std::string& name, const std::string& description,
                     const std::string& shaderType, const std::string& vertexFile, 
                     const std::string& fragmentFile);
    
    // Valoraciones
    bool rateResource(int resourceId, int rating);
    
    // Getters
    bool isAuthenticated() const { return !accessToken.empty(); }
    std::string getAccessToken() const { return accessToken; }
};

#endif // RESOURCE_HUB_CLIENT_H
