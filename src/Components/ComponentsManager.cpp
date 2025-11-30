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

void ComponentsManager::RegisterComponent(Component *component, const std::string& label)
{
    Logging::Message("[ComponentsManager] Register Profile measure for: %s", component->getLabel().c_str());

    component->setId((int) components.size());
    component->setLabel(label);

    components.push_back(component);
    Profiler::get()->ResetMeasure(Profiler::get()->getComponentMeasures(), component->getLabel() + "_pre");
    Profiler::get()->ResetMeasure(Profiler::get()->getComponentMeasures(),component->getLabel() + "_update");
    Profiler::get()->ResetMeasure(Profiler::get()->getComponentMeasures(), component->getLabel() + "_post");
}

ComponentsManager::~ComponentsManager()
{
    for (auto component : components) {
        delete component;
    }
}