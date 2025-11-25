#include "../../include/Components/ComponentsManager.h"
#include "../../include/Misc/Logging.h"

ComponentsManager *ComponentsManager::instance = nullptr;

ComponentsManager *ComponentsManager::get()
{
    if (instance == nullptr) {
        instance = new ComponentsManager();
    }

    return instance;
}

void ComponentsManager::registerComponent(Component *component, const std::string& label)
{
    component->setId(components.size());
    component->setLabel(label);

    components.push_back(component);

    Logging::Message("Register component: %s", component->getLabel().c_str());
}

ComponentsManager::~ComponentsManager()
{
    for (auto component : components) {
        delete component;
    }
}


