#ifndef BRAKEZA3D_GUIADDONRESOURCEHUB_H
#define BRAKEZA3D_GUIADDONRESOURCEHUB_H


#include <string>
#include <vector>

#include "../../HttpClient/ResourceHubClient.h"

class GUIAddonResourceHub {
private:
    ResourceHubClient* client;
    
    // Estado de autenticación
    bool isLoggedIn;
    bool showLoginWindow;
    bool showBrowserWindow;
    
    // Formulario de login
    char username[128];
    char password[128];
    std::string loginError;
    bool isLoggingIn;
    
    // Búsqueda y filtros
    char searchQuery[256];
    int selectedType;  // 0=all, 1=script, 2=shader
    int selectedCategory;  // 0=all, 1=local, 2=global, etc
    int currentPage;
    int totalPages;
    
    // Recursos
    struct ResourceInfo {
        int id;
        std::string name;
        std::string description;
        std::string type;
        std::string author;
        float rating;
        int ratingCount;
        std::string scriptType;
        std::string shaderType;
    };
    
    std::vector<ResourceInfo> resources;
    int selectedResourceIndex;
    bool isLoadingResources;
    
    // Detalle de recurso
    cJSON* selectedResourceDetail;
    bool showResourceDetail;
    
    // Métodos privados
    void renderLoginWindow();
    void renderBrowserWindow();
    void renderResourceList();
    void renderResourceDetail();
    void renderSearchFilters();
    
    void performLogin();
    void performLogout();
    void loadResources();
    void loadResourceDetail(int resourceId);
    void parseResourcesFromJSON(cJSON* json);
    void clearResourceDetail();

public:
    GUIAddonResourceHub();
    ~GUIAddonResourceHub();
    
    void render();
    void showLogin();
    void showBrowser();
    bool isAuthenticated() const { return isLoggedIn; }
};

#endif //BRAKEZA3D_GUIADDONRESOURCEHUB_H
