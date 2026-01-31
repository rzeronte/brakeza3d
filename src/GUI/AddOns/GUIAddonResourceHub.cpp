#include "../../../include/GUI/AddOns/GUIAddonResourceHub.h"
#include "imgui.h"
#include <cstring>
#include <iostream>
#include <thread>
#include <chrono>
#include <map>

#include "../../../include/Brakeza.h"
#include "../../../include/GUI/GUI.h"
#include "../../../include/Config.h"
#include "../../../include/Components/Components.h"
#include "../../../include/HttpClient/ResourceHubClient.h"
#include "../../../include/GUI/Objects/FileSystemGUI.h"
#include "../../../include/Render/Drawable.h"

GUIAddonResourceHub::GUIAddonResourceHub()
{
    client = new ResourceHubClient(Config::get()->URL_ASSETS_HUB_API);
    
    isLoggedIn = false;
    showLoginWindow = false;
    showBrowserWindow = false;
    
    std::memset(username, 0, sizeof(username));
    std::memset(password, 0, sizeof(password));
    loginError = "";
    isLoggingIn = false;
    
    std::memset(searchQuery, 0, sizeof(searchQuery));
    selectedType = 0;
    selectedCategory = 0;
    currentPage = 1;
    totalPages = 1;
    
    selectedResourceIndex = -1;
    isLoadingResources = false;
    selectedResourceDetail = nullptr;
    showResourceDetail = false;
}

GUIAddonResourceHub::~GUIAddonResourceHub() {
    clearResourceDetail();
    delete client;
}

void GUIAddonResourceHub::showLogin() {
    showLoginWindow = true;
}

void GUIAddonResourceHub::showBrowser() {
    if (isLoggedIn) {
        showBrowserWindow = true;
        if (resources.empty()) {
            loadResources();
        }
    } else {
        showLogin();
    }
}

void GUIAddonResourceHub::render() {
    if (showLoginWindow) {
        renderLoginWindow();
    }
    
    if (showBrowserWindow && isLoggedIn) {
        renderBrowserWindow();
    }
    
    if (showResourceDetail) {
        renderResourceDetail();
    }
}

void GUIAddonResourceHub::renderLoginWindow() {
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
                            ImGuiCond_FirstUseEver, ImVec2(0.5f, 0.5f));

    ImGui::SetNextWindowBgAlpha(0.99f);

    if (ImGui::Begin("Assets Hub - Login", &showLoginWindow, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking)) {
        // Username
        ImGui::Text("Username:");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##username", username, sizeof(username));
        ImGui::Spacing();

        // Password
        ImGui::Text("Password:");
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##password", password, sizeof(password), ImGuiInputTextFlags_Password);
        ImGui::Spacing();
        ImGui::Spacing();

        // Error message
        if (!loginError.empty()) {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "%s", loginError.c_str());
            ImGui::Spacing();
        }

        // Buttons
        if (isLoggingIn) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Logging in...");
        } else {
            GUI::ImageButtonNormal(IconGUI::CANCEL, "Close", [&] {
                showLoginWindow = false;
                loginError = "";
            });
            // alineamos dcha
            ImGui::SameLine();
            float buttonWidth = 85.0f; // Ajusta al tamaño de tu botón
            ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x - buttonWidth, 0));
            ImGui::SameLine();
            GUI::ImageButtonNormal(IconGUI::SESSION_OPEN, "Login", [&] {
                performLogin();
            });

        }

        ImGui::Spacing();
        ImGui::Separator();
        Drawable::ImGuiLink("Don't have any account? Register!", Config::get()->URL_ASSETS_HUB_URL.c_str());
        Drawable::ImGuiLink("Forgot your password?", Config::get()->URL_ASSETS_HUB_URL.c_str());
    }
    ImGui::End();
}

void GUIAddonResourceHub::renderBrowserWindow()
{
    ImGui::SetNextWindowSize(ImVec2(900, 600), ImGuiCond_FirstUseEver);

    ImGui::SetNextWindowBgAlpha(0.99f);

    if (ImGui::Begin(Config::get()->URL_ASSETS_HUB_URL.c_str(), &showBrowserWindow, ImGuiWindowFlags_NoDocking)) {
        // Crear un espacio invisible que empuje los botones a la derecha
        ImGui::Dummy(ImVec2(ImGui::GetContentRegionAvail().x - 170.0f, 0));
        ImGui::SameLine();

        GUI::ImageButtonNormal(IconGUI::SESSION_CLOSE, "Logout", [&] {
            performLogout();
        });
        ImGui::SameLine();
        GUI::ImageButtonNormal(IconGUI::CANCEL, "Close", [&] {
            showBrowserWindow = false;
        });
        ImGui::Separator();
        ImGui::Spacing();

        // Search and filters
        renderSearchFilters();
        
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        
        // Resource list
        renderResourceList();
        
        ImGui::Spacing();
        ImGui::Separator();
        
        // Pagination - centrar texto verticalmente con botones
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Page %d of %d", currentPage, totalPages);
        ImGui::SameLine();
        
        if (currentPage > 1) {
            if (ImGui::Button("< Previous")) {
                currentPage--;
                loadResources();
            }
            ImGui::SameLine();
        }
        
        if (currentPage < totalPages) {
            if (ImGui::Button("Next >")) {
                currentPage++;
                loadResources();
            }
        }
    }
    ImGui::End();
}

void GUIAddonResourceHub::renderSearchFilters() {
    // Search box
    ImGui::AlignTextToFramePadding(); // Centrar verticalmente
    ImGui::Text("Search:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(150);
    if (ImGui::InputText("##search", searchQuery, sizeof(searchQuery), ImGuiInputTextFlags_EnterReturnsTrue)) {
        currentPage = 1;
        loadResources();
    }
    ImGui::SameLine();

    GUI::ImageButtonNormal(IconGUI::SEARCH, "Search", [&] {
        currentPage = 1;
        loadResources();
    });

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);

    // Type filter
    ImGui::AlignTextToFramePadding(); // Centrar verticalmente
    ImGui::Text("Type:");
    ImGui::SameLine();
    ImGui::SetNextItemWidth(120);
    const char* types[] = { "All", "Scripts", "Shaders" };
    if (ImGui::Combo("##type", &selectedType, types, IM_ARRAYSIZE(types))) {
        currentPage = 1;
        loadResources();
    }

    ImGui::SameLine();

    // Refresh button
    GUI::ImageButtonNormal(IconGUI::CLEAR_FILTERS, "Clear filters", [&] {
        loadResources();
    });
}

void GUIAddonResourceHub::renderResourceList() {
    if (isLoadingResources) {
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "Loading resources...");
        return;
    }
    
    if (resources.empty()) {
        Drawable::WarningMessage("No resources found. Try searching or change filters.");
        return;
    }
    
    ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(8.0f, 8.0f));

    if (ImGui::BeginTable("ResourcesTable", 5, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 350))) {
        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Category", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Author", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("Rating", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        for (size_t i = 0; i < resources.size(); i++) {
            const ResourceInfo& res = resources[i];

            ImGui::TableNextRow();

            // Name (clickable)
            ImGui::TableSetColumnIndex(0);
            if (ImGui::Selectable(res.name.c_str(), selectedResourceIndex == (int)i, ImGuiSelectableFlags_SpanAllColumns)) {
                selectedResourceIndex = i;
                loadResourceDetail(res.id);
            }

            // Type
            ImGui::TableSetColumnIndex(1);
            if (res.type == "script") {
                ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "Script");
            } else {
                ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "Shader");
            }

            // Category
            ImGui::TableSetColumnIndex(2);
            ImGui::Text("%s", res.type == "script" ? res.scriptType.c_str() : res.shaderType.c_str());

            // Author
            ImGui::TableSetColumnIndex(3);
            ImGui::Image(FileSystemGUI::Icon(IconGUI::HUB_ASSET_AUTHOR), GUIType::Sizes::ICON_SIZE_MENUS); ImGui::SameLine();
            ImGui::SameLine();
            ImGui::Text("%s", res.author.c_str());

            // Rating
            ImGui::TableSetColumnIndex(4);
            {
                auto starOn = FileSystemGUI::Icon(IconGUI::STAR_ON);
                auto starOff = FileSystemGUI::Icon(IconGUI::STAR_OFF);
                ImVec2 starSize = ImVec2(12, 12);

                for (int s = 0; s < 5; s++) {
                    if (s > 0) ImGui::SameLine(0, 1);
                    if (s < (int)res.rating) {
                        ImGui::Image(starOn, starSize);
                    } else {
                        ImGui::Image(starOff, starSize);
                    }
                }
                ImGui::SameLine();
                ImGui::Text("(%d)", res.ratingCount);
            }
        }

        ImGui::EndTable();
    }

    ImGui::PopStyleVar();
}

void GUIAddonResourceHub::performLogin()
{
    if (strlen(username) == 0 || strlen(password) == 0) {
        loginError = "Please enter username and password";
        return;
    }
    
    isLoggingIn = true;
    loginError = "";
    
    if (client->login(username, password)) {
        isLoggedIn = true;
        showLoginWindow = false;
        showBrowserWindow = true;
        isLoggingIn = false;
        
        // Clear password
        std::memset(password, 0, sizeof(password));
        
        // Load initial resources
        loadResources();
    } else {
        loginError = "Invalid credentials. Try demo_user / 123456";
        isLoggingIn = false;
    }
}

void GUIAddonResourceHub::performLogout()
{
    client->logout();
    isLoggedIn = false;
    showBrowserWindow = false;
    resources.clear();
    clearResourceDetail();
    
    std::memset(username, 0, sizeof(username));
    std::memset(password, 0, sizeof(password));
}

void GUIAddonResourceHub::loadResources()
{
    isLoadingResources = true;
    resources.clear();
    
    const char* typeFilter = "all";
    if (selectedType == 1) typeFilter = "script";
    else if (selectedType == 2) typeFilter = "shader";
    
    cJSON* result = client->getResources(currentPage, 9, searchQuery, typeFilter, "all");
    
    if (result) {
        parseResourcesFromJSON(result);
        
        // Get total pages
        cJSON* totalPagesItem = cJSON_GetObjectItem(result, "totalPages");
        if (totalPagesItem) {
            totalPages = totalPagesItem->valueint;
        }
        
        cJSON_Delete(result);
    }
    
    isLoadingResources = false;
}

void GUIAddonResourceHub::loadResourceDetail(int resourceId) {
    clearResourceDetail();
    
    selectedResourceDetail = client->getResource(resourceId);
    
    if (selectedResourceDetail) {
        showResourceDetail = true;
    }
}

void GUIAddonResourceHub::parseResourcesFromJSON(cJSON* json) {
    cJSON* resourcesArray = cJSON_GetObjectItem(json, "resources");
    
    if (!resourcesArray || !cJSON_IsArray(resourcesArray)) {
        return;
    }
    
    int count = cJSON_GetArraySize(resourcesArray);
    
    for (int i = 0; i < count; i++) {
        cJSON* item = cJSON_GetArrayItem(resourcesArray, i);
        
        ResourceInfo info;
        
        cJSON* id = cJSON_GetObjectItem(item, "id");
        cJSON* name = cJSON_GetObjectItem(item, "name");
        cJSON* description = cJSON_GetObjectItem(item, "description");
        cJSON* type = cJSON_GetObjectItem(item, "type");
        cJSON* author = cJSON_GetObjectItem(item, "author");
        cJSON* rating = cJSON_GetObjectItem(item, "average_rating");
        cJSON* ratingCount = cJSON_GetObjectItem(item, "rating_count");
        cJSON* scriptType = cJSON_GetObjectItem(item, "script_type");
        cJSON* shaderType = cJSON_GetObjectItem(item, "shader_type");
        
        if (id) info.id = id->valueint;
        if (name) info.name = name->valuestring;
        if (description) info.description = description->valuestring;
        if (type) info.type = type->valuestring;
        if (author) info.author = author->valuestring;
        if (rating) info.rating = rating->valuedouble;
        if (ratingCount) info.ratingCount = ratingCount->valueint;
        if (scriptType) info.scriptType = scriptType->valuestring;
        if (shaderType) info.shaderType = shaderType->valuestring;
        
        resources.push_back(info);
    }
}

void GUIAddonResourceHub::clearResourceDetail() {
    if (selectedResourceDetail) {
        cJSON_Delete(selectedResourceDetail);
        selectedResourceDetail = nullptr;
    }
    showResourceDetail = false;
}

void GUIAddonResourceHub::renderResourceDetail() {
    if (!selectedResourceDetail) return;

    ImGui::SetNextWindowSize(ImVec2(700, 600), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowBgAlpha(0.99f);

    if (ImGui::Begin("Resource Details", &showResourceDetail,  ImGuiWindowFlags_NoDocking)) {
        cJSON* name = cJSON_GetObjectItem(selectedResourceDetail, "name");
        cJSON* description = cJSON_GetObjectItem(selectedResourceDetail, "description");
        cJSON* type = cJSON_GetObjectItem(selectedResourceDetail, "type");
        cJSON* author = cJSON_GetObjectItem(selectedResourceDetail, "author");
        cJSON* rating = cJSON_GetObjectItem(selectedResourceDetail, "average_rating");
        cJSON* ratingCount = cJSON_GetObjectItem(selectedResourceDetail, "rating_count");
        cJSON* files = cJSON_GetObjectItem(selectedResourceDetail, "files");
        cJSON* id = cJSON_GetObjectItem(selectedResourceDetail, "id");

        // Title
        if (name) {
            ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "%s", name->valuestring);
            ImGui::SameLine();
            ImGui::Text("by");
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "<%s>", author->valuestring);

        }
        ImGui::Separator();
        ImGui::Spacing();

        // Type and author
        ImGui::Text("Type:");
        ImGui::SameLine();
        if (type) {
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", type->valuestring);
        }

        ImGui::Spacing();

        if (rating && ratingCount) {
            float ratingValue = static_cast<float>(rating->valuedouble);
            auto starOn = FileSystemGUI::Icon(IconGUI::STAR_ON);
            auto starOff = FileSystemGUI::Icon(IconGUI::STAR_OFF);

            // Tamaño de las estrellas (ajusta según tus imágenes)
            ImVec2 starSize = ImVec2(16, 16);  // o GUIType::Sizes::ICON_SIZE_MENUS

            // Mostrar estrellas visuales
            for (int i = 0; i < 5; i++) {
                if (i > 0) ImGui::SameLine();

                if (i < (int)ratingValue) {
                    ImGui::Image(starOn, starSize);
                } else {
                    ImGui::Image(starOff, starSize);
                }
            }

            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "(%d ratings)", ratingCount->valueint);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Description
        ImGui::Text("Description:");
        ImGui::Spacing();
        if (description) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.9f, 0.9f, 0.9f, 1.0f));
            ImGui::TextWrapped("%s", description->valuestring);
            ImGui::PopStyleColor();
        }

        ImGui::Spacing();
        ImGui::Separator();

        // Files list with individual download buttons
        if (files && cJSON_IsArray(files)) {
            int fileCount = cJSON_GetArraySize(files);

            // Download All button
            if (fileCount > 1) {
                if (downloadingAll) {
                    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                    ImGui::Button("Downloading...", ImVec2(150, 0));
                    ImGui::PopStyleColor();
                } else {
                    GUI::ImageButtonNormal(IconGUI::DOWNLOAD_ALL_RESOURCE, "Download ALL", [&] {
                        downloadAllFiles(id->valueint, files);
                    });
                }
                ImGui::Spacing();
            }

            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(10.0f, 8.0f)); // Más padding
            if (ImGui::BeginTable("FilesTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY, ImVec2(0, 200))) {
                ImGui::TableSetupColumn("Filename", ImGuiTableColumnFlags_WidthStretch);
                ImGui::TableSetupColumn("Action", ImGuiTableColumnFlags_WidthFixed);

                for (int i = 0; i < fileCount; i++) {
                    cJSON* file = cJSON_GetArrayItem(files, i);
                    cJSON* filename = cJSON_GetObjectItem(file, "filename");

                    if (filename) {
                        std::string fname = filename->valuestring;

                        ImGui::TableNextRow();

                        // Columna: Nombre del archivo
                        ImGui::TableSetColumnIndex(0);
                        ImGui::AlignTextToFramePadding(); // Alineación vertical
                        ImGui::Text("%s", fname.c_str());

                        // Columna: Botón de descarga
                        ImGui::TableSetColumnIndex(1);

                        std::string buttonId = "##download_" + std::to_string(i);

                        if (downloadingFiles[fname]) {
                            // Downloading...
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                            ImGui::Button("Downloading...", ImVec2(-FLT_MIN, 0)); // -FLT_MIN ocupa todo el ancho
                            ImGui::PopStyleColor();

                        } else if (downloadSuccess.count(fname) && downloadSuccess[fname]) {
                            // Success
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.7f, 0.0f, 1.0f));
                            ImGui::Button("Downloaded", ImVec2(-FLT_MIN, 0));
                            ImGui::PopStyleColor();

                        } else if (downloadErrors.count(fname)) {
                            // Error
                            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.0f, 0.0f, 1.0f));
                            if (ImGui::Button(("Error" + buttonId).c_str(), ImVec2(50, 0))) {
                                // Mostrar error en tooltip
                            }
                            ImGui::PopStyleColor();

                            if (ImGui::IsItemHovered()) {
                                ImGui::SetTooltip("%s", downloadErrors[fname].c_str());
                            }

                            // Botón retry en la misma línea
                            ImGui::SameLine();
                            if (ImGui::SmallButton(("Retry" + buttonId).c_str())) {
                                downloadFile(id->valueint, fname);
                            }

                        } else {
                            // Download button
                            GUI::ImageButtonNormal(IconGUI::DOWNLOAD_RESOURCE, "Download file", [&, fname, id] {
                                downloadFile(id->valueint, fname);
                            });
                        }
                    }
                }

                ImGui::EndTable();
            }

            ImGui::PopStyleVar();
            } else {
                ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No files available");
            }
        ImGui::Spacing();

        // ============================================
        // RATING
        // ============================================

        static int userRating = 5;
        ImGui::Text("Rate this resource:");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::SliderInt("##rating", &userRating, 1, 5, "%d stars");
        ImGui::SameLine();
        GUI::ImageButtonNormal(IconGUI::HUB_ASSET_SUBMIT_RATING, "Submit rating", [&, id] {
            if (id) {
                if (client->rateResource(id->valueint, userRating)) {
                    std::cout << "Rating submitted successfully!" << std::endl;
                    loadResourceDetail(id->valueint);  // Reload to see updated rating
                } else {
                    std::cout << "Failed to submit rating" << std::endl;
                }
            }
        });

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // Close button
        float buttonWidth = 75.0f;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - buttonWidth - ImGui::GetStyle().WindowPadding.x);

        GUI::ImageButtonNormal(IconGUI::CANCEL, "Close", [&] {
            showResourceDetail = false;
        });
    }
    ImGui::End();
}

// ============================================
// FUNCIÓN HELPER: Descargar un archivo individual
// ============================================

void GUIAddonResourceHub::downloadFile(int resourceId, const std::string& filename) {
    // Marcar como descargando
    downloadingFiles[filename] = true;
    downloadSuccess[filename] = false;
    downloadErrors.erase(filename);

    // Construir ruta de salida
    std::string outputPath = downloadPath + filename;

    std::cout << "Downloading: " << filename << " to " << outputPath << std::endl;

    // Realizar descarga en thread separado para no bloquear UI
    std::thread([this, resourceId, filename, outputPath]() {
        bool success = client->downloadResourceFile(resourceId, filename, outputPath);

        // Actualizar estado
        downloadingFiles[filename] = false;

        if (success) {
            downloadSuccess[filename] = true;
            std::cout << "✓ Downloaded: " << filename << std::endl;
        } else {
            downloadErrors[filename] = "Download failed. Check console for details.";
            std::cout << "✗ Failed: " << filename << std::endl;
        }
    }).detach();
}

// ============================================
// FUNCIÓN HELPER: Descargar todos los archivos
// ============================================

void GUIAddonResourceHub::downloadAllFiles(int resourceId, cJSON* files) {
    if (!files || !cJSON_IsArray(files)) return;

    downloadingAll = true;

    int fileCount = cJSON_GetArraySize(files);

    std::cout << "Downloading all files (" << fileCount << ")..." << std::endl;

    // Descargar todos en thread separado
    std::thread([this, resourceId, files, fileCount]() {
        for (int i = 0; i < fileCount; i++) {
            cJSON* file = cJSON_GetArrayItem(files, i);
            cJSON* filename = cJSON_GetObjectItem(file, "filename");

            if (filename) {
                std::string fname = filename->valuestring;
                std::string outputPath = downloadPath + fname;

                downloadingFiles[fname] = true;

                bool success = client->downloadResourceFile(resourceId, fname, outputPath);

                downloadingFiles[fname] = false;

                if (success) {
                    downloadSuccess[fname] = true;
                } else {
                    downloadErrors[fname] = "Download failed";
                }

                // Pequeña pausa entre descargas
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        downloadingAll = false;
        std::cout << "✓ All downloads completed" << std::endl;
    }).detach();
}
